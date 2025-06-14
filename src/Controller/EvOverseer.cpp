#include "EvOverseer.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <random>
#include <thread>
#include <vector>

#include "Controller/Sim.h"
#include "Misc/Utils.h"
#include "Model/Agents/Predator.h"
#include "Model/Agents/Prey.h"
#include "Model/Agents/config.h"
#include "Model/Evo/MarkovBrain.h"
#include "View/MainView.h"
#include "fmt/format.h"
#include "toml++/toml.h"


using namespace sim;

EvOverseer::EvOverseer()
    : prng{[] {
          std::random_device rd;
          std::seed_seq      seq{rd(), rd(), rd(), rd()};
          return decltype(prng){seq};
      }()}
{
}

void EvOverseer::load_settings(std::filesystem::path settings_filepath)
{
    try
    {
        settings = toml::parse_file(settings_filepath.c_str());
    }
    catch (const toml::parse_error& err)
    {
        fmt::println(stderr, "Error parsing file '{}':\n{}\n ({}). Using default settings",
                     *err.source().path, err.description(), err.source().begin);

        std::ofstream settings_file{"settings.toml"};
        settings_file << settings;
    }
}

void EvOverseer::sim()
{
    std::ofstream fitness_file("fitness.txt");

    const auto& viewport   = *settings.get("viewport")->as_table();
    const auto& simulation = *settings.get("simulation")->as_table();

    const auto universe_width  = simulation["universe"]["width"].ref<int64_t>();
    const auto universe_height = simulation["universe"]["height"].ref<int64_t>();
    const auto w_scale         = viewport["width"].ref<double>() / universe_width;
    const auto h_scale         = viewport["height"].ref<double>() / universe_height;

    std::vector<MarkovBrain> pred_mb_pool;
    std::vector<MarkovBrain> prey_mb_pool;

    int generations = simulation["generations"].ref<int64_t>();
    int nb_threads  = simulation["threads"].ref<int64_t>();

    double pred_fitness_geom_mean = 0;
    double prey_fitness_geom_mean = 0;

    static constexpr auto build_markov_brain = []<typename T>(config::Prey const& config) {
        auto const& mb_conf = config.markov_brain;

        MarkovBrain mb;
        if (std::filesystem::exists(mb_conf.file_to_load))
        {
            std::ifstream evolved_mb_file{mb_conf.file_to_load};
            evolved_mb_file >> mb;

            fmt::println("Loaded MB file: \"{}\"", mb_conf.file_to_load.string());
        }
        else
        {
            fmt::println("Unable to load MB file: \"{}\"", mb_conf.file_to_load.string());

            mb = {mb_conf.max_inputs, mb_conf.max_outputs,
                  std::popcount(static_cast<uint8_t>(config.attributes.targets)) *
                          config.attributes.nb_retina_cells +
                      T::__nb_actions,mb_conf.ancestor_genes};
        }
        return mb;
    };

    const size_t pool_fill_size = simulation["pool size"].as_integer()->get();

    auto agents_config = config::parse_config(settings);
    for (auto const& agent_config : agents_config)
    {
        std::visit(overloaded{
                       [&](config::Prey const& config) {
                           prey_mb_pool = {
                               pool_fill_size,
                               build_markov_brain.template operator()<sim::Prey>(config)
                           };
                       },
                       [&](config::Predator const& config) {
                           pred_mb_pool = {
                               pool_fill_size,
                               build_markov_brain.template operator()<sim::Predator>(config)
                           };
                       },
                   },
                   agent_config);
    }

    std::optional<MainView> view;
    if (!viewport["headless"].ref<bool>())
    {
        // clang-format off
        view.emplace(viewport["width"].ref<int64_t>(),
                     viewport["height"].ref<int64_t>(),
                     w_scale,
                     h_scale);
        // clang-format on
    }
    for (int generation = 0; generation < generations; ++generation)
    {
        std::ranges::shuffle(pred_mb_pool, prng);
        std::ranges::shuffle(prey_mb_pool, prng);

        fmt::println("Generation {}/{}:", generation + 1, generations);

        std::vector<std::string> workers_output{pred_mb_pool.size()};

        auto const batch_size = pred_mb_pool.size() / nb_threads;

        auto run_sim = [&](int i) {
            for (size_t j = 0; j < batch_size; ++j)
            {
                auto& pred_mb = prey_mb_pool[i * batch_size + j];
                Sim   thread_sim;
                if (i == 0 && view)
                {
                    thread_sim.set_view(*view);
                }
                if (!thread_sim.run())
                {
                    workers_output[i * batch_size + j] = "User interrupted simulation";
                    break;
                }
                workers_output[i * batch_size + j] = fmt::format(
                    "generation {:>6}, predator {:>6}, seed {:>16}, fitness {:>9}, prey "
                    "{:>6}, seed {:>16}, fitness {:>9}\n",
                    generation, j, mb_pred_seed, pred_fitness_val, j, mb_prey_seed,
                    prey_fitness_val);
            }
        };

        using namespace std::chrono;
        steady_clock::time_point start, end;
        {

            start = steady_clock::now();
            std::vector<std::jthread> new_workers;
            for (int i = 1; i < nb_threads; ++i)
            {
                new_workers.emplace_back(run_sim, i);
            }
            run_sim(0);
            end = steady_clock::now();
        }

        // Increase mutation rate depending on the fitness
        // TODO: per agent mutation rate
        for (auto const& agent : {"predator", "prey"})
        {
            if (settings["simulation"][fmt::format("evolve {}", agent)].as_boolean()->get())
            {
                double fitness_geom_mean = 1;
                for (const auto [_, fitness] : pred_seeds_with_fitness)
                {
                    fitness_geom_mean += std::log(fitness > 0 ? fitness : 1);
                }
                fitness_geom_mean /= pred_seeds_with_fitness.size();
                fitness_geom_mean  = std::exp(fitness_geom_mean);

                //   if (tmp_pred_fit_geom_mean < pred_fitness_geom_mean * 1.01) {
                //     MarkovBrain::increase_mutation_rate();
                //   } else {
                //     MarkovBrain::decrease_mutation_rate();
                //   }
                pred_fitness_geom_mean = fitness_geom_mean;
                const auto mb_dirname  = fmt::format("{}_mb/{}", agent, generation);
                std::filesystem::create_directories(mb_dirname);

                for (auto const& agent_mb : _pred_mb_pool)
                {
                    const auto seed    = agent_mb.current_seed();
                    const auto fitness = pred_seeds_with_fitness[seed];

                    std::ofstream evolved_mb_file{
                        fmt::format("{}/{}_{}.txt", mb_dirname, fitness, seed)};
                    evolved_mb_file << agent_mb;
                }
                _moran_process(pred_seeds_with_fitness, _pred_mb_pool);
            }
        }

        fmt::print("  - duration: {}s\n"
                   "  - predator mean (geom) fitness: {:6.2f}\n"
                   "  - prey mean (geom) fitness:     {:6.2f}\n"
                   "  - mutation rate: {}\n",
                   duration_cast<seconds>(end - start).count(), pred_fitness_geom_mean,
                   prey_fitness_geom_mean, MarkovBrain::get_mutation_rate());
    }

    fitness_file.close();
}

uint64_t EvOverseer::_stochastic_acceptance(
    std::unordered_map<uint64_t, uint32_t> const& seeds_with_fitness)
{
    uint32_t total_generation_fitness = 1;
    for (auto&& [_, fitness] : seeds_with_fitness)
    {
        total_generation_fitness += fitness;
    }

    auto const cursor = std::uniform_int_distribution<uint32_t>{0, total_generation_fitness}(prng);
    uint32_t   cumulated_fitness = 1;
    for (auto&& [seed, fitness] : seeds_with_fitness)
    {
        cumulated_fitness += fitness;
        if (cursor <= cumulated_fitness)
        {
            return seed;
        }
    }
    return std::begin(seeds_with_fitness)->first;
}

void EvOverseer::_moran_process(std::unordered_map<uint64_t, uint32_t> const& mb_seeds_fit,
                                std::vector<MarkovBrain>&                     population)
{
    std::vector<MarkovBrain> offsprings = population;
    for (uint32_t i = 0; i < population.size(); ++i)
    {
        offsprings[i] = *std::ranges::find(population, _stochastic_acceptance(mb_seeds_fit),
                                           &MarkovBrain::current_seed);
    }

    population = std::move(offsprings);
    for (auto& evolving_mb : population)
    {
        evolving_mb.mutation(*settings.get("genome mutation")->as_table());
    }
}

std::ostream& ::sim::operator<<(std::ostream & os, EvOverseer const& lightsim)
{
    os << lightsim.settings << std::endl << std::endl;

    return os;
}
