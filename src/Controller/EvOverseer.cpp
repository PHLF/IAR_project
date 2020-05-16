#include "EvOverseer.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iterator>
#include <sstream>
#include <streambuf>
#include <string_view>
#include <thread>

#include "fmt/format.h"
#include "toml++/toml.h"
#include "pcg_random.hpp"

using namespace sim;

EvOverseer::EvOverseer() {}

EvOverseer::~EvOverseer() {}

void EvOverseer::load_settings(std::filesystem::path file_settings) {
  if (!std::filesystem::exists(file_settings)) {
    std::ofstream default_settings("settings.toml");

    default_settings << _settings << std::endl;

    default_settings.close();
  } else {
    try {
      std::ifstream settings(file_settings);
      std::string str;

      settings.seekg(0, std::ios::end);
      str.reserve(settings.tellg());
      settings.seekg(0, std::ios::beg);

      str.assign((std::istreambuf_iterator<char>(settings)),
                 std::istreambuf_iterator<char>());
      auto tbl = toml::parse(str);
      _settings = std::move(tbl);
    } catch (const toml::parse_error& err) {
      //      fmt::print(stderr, "Error parsing file '{}':\n{}\n ({})",
      //                 *err.source().path, err.description(),
      //                 err.source().begin);
    }
  }
  _setup_sim();
}

void EvOverseer::_setup_sim() {
  std::ifstream evolved_mb_file;

  const auto& predator = *_settings.get("predator")->as_table();

  const auto nb_nodes_for_predators =
      predator["sight"]["retina cells"].as_integer()->get() +
      predator["memory cells"].as_integer()->get() + Agent::nb_actions();
  const auto pred_mb_max_inputs =
      predator["markov brain"]["max inputs"].as_integer()->get();
  const auto pred_mb_max_outputs =
      predator["markov brain"]["max outputs"].as_integer()->get();
  const auto pred_mb_nb_ancestor_genes =
      predator["markov brain"]["ancestor genes"].as_integer()->get();

  const auto& prey = *_settings.get("prey")->as_table();

  const auto nb_nodes_for_preys =
      // Prey retina has two layers (one sensitive to others Preys and one
      // sensitive to predators)
      prey["sight"]["retina cells by agent type"].as_integer()->get() * 2 +
      prey["memory cells"].as_integer()->get() + Agent::nb_actions();
  const auto prey_mb_max_inputs =
      prey["markov brain"]["max inputs"].as_integer()->get();
  const auto prey_mb_max_outputs =
      prey["markov brain"]["max outputs"].as_integer()->get();
  const auto prey_mb_nb_ancestor_genes =
      prey["markov brain"]["ancestor genes"].as_integer()->get();

  const auto& simulation = *_settings.get("simulation")->as_table();

  const auto pred_mb_file =
      predator["markov brain"]["file to load"].as_string()->get();
  if (std::filesystem::exists(pred_mb_file)) {
    MarkovBrain pred_mb;
    evolved_mb_file.open(pred_mb_file, std::ios::in);
    evolved_mb_file >> pred_mb;
    evolved_mb_file.close();

    const auto pool_fill_size = simulation["pool size"].as_integer()->get();
    for (uint32_t i = 0; i < pool_fill_size; ++i) {
      _pred_mb_pool.push_back(pred_mb);
    }
  }

  const auto prey_mb_file =
      prey["markov brain"]["file to load"].as_string()->get();
  if (std::filesystem::exists(prey_mb_file)) {
    MarkovBrain prey_mb;
    evolved_mb_file.open(prey_mb_file, std::ios::in);
    evolved_mb_file >> prey_mb;
    evolved_mb_file.close();

    const auto pool_fill_size = simulation["pool size"].as_integer()->get();
    for (uint32_t i = 0; i < pool_fill_size; ++i) {
      _prey_mb_pool.push_back(prey_mb);
    }
  }

  {
    const auto pool_fill_size =
        simulation["pool size"].as_integer()->get() - _pred_mb_pool.size();

    for (uint32_t i = 0; i < pool_fill_size; ++i) {
      _pred_mb_pool.emplace_back(
          MarkovBrain(pred_mb_max_inputs, pred_mb_max_outputs,
                      nb_nodes_for_predators, pred_mb_nb_ancestor_genes));
    }
  }
  {
    const auto pool_fill_size =
        simulation["pool size"].as_integer()->get() - _prey_mb_pool.size();

    for (uint32_t i = 0; i < pool_fill_size; ++i) {
      _prey_mb_pool.emplace_back(
          MarkovBrain(prey_mb_max_inputs, prey_mb_max_outputs,
                      nb_nodes_for_preys, prey_mb_nb_ancestor_genes));
    }
  }
}

EvOverseer::OptSimResult EvOverseer::_run_thread(
    uint32_t thread_number,
    uint32_t generation,
    std::vector<MarkovBrain>& pred_pool,
    std::vector<MarkovBrain>& prey_pool) {
  uint32_t loop_range_begin = 0;
  uint32_t loop_range_end = 0;

  std::stringstream thread_output;

  std::unordered_map<uint64_t, uint32_t> pred_seeds_with_fitness;
  std::unordered_map<uint64_t, uint32_t> prey_seeds_with_fitness;

  uint32_t pool_size = static_cast<uint32_t>(pred_pool.size());
  uint32_t threads = _settings["simulation"]["threads"].as_integer()->get();

  loop_range_begin = thread_number * (pool_size / threads);
  loop_range_end = (thread_number + 1) * (pool_size / threads);

  std::vector<MarkovBrain> local_pred_pool{
      std::begin(pred_pool) + loop_range_begin,
      std::begin(pred_pool) + loop_range_end};
  std::vector<MarkovBrain> local_prey_pool{
      std::begin(prey_pool) + loop_range_begin,
      std::begin(prey_pool) + loop_range_end};

  static constexpr auto get_mbs_pair =
      [](std::vector<MarkovBrain>& pred_pool_,
         std::vector<MarkovBrain>& prey_pool_) {
        auto pred_mb = pred_pool_.back();
        pred_pool_.pop_back();

        auto prey_mb = prey_pool_.back();
        prey_pool_.pop_back();

        return std::make_pair(pred_mb, prey_mb);
      };

  auto [pred_mb0, prey_mb0] = get_mbs_pair(local_pred_pool, local_prey_pool);

  Sim thread_sim(_settings, pred_mb0, prey_mb0);
  thread_sim.set_view(_view.get());

  for (uint32_t j = loop_range_begin; j < loop_range_end; ++j) {
    if (j != loop_range_begin) {
      auto [pred_mb, prey_mb] = get_mbs_pair(local_pred_pool, local_prey_pool);

      thread_sim.pred_mb = std::move(pred_mb);
      thread_sim.prey_mb = std::move(prey_mb);
    }

    if (!thread_sim.run()) {
      return {};
    }

    uint32_t pred_fitness_val = thread_sim.eval_pred();
    uint32_t prey_fitness_val = thread_sim.eval_prey();
    uint64_t mb_pred_seed = thread_sim.pred_mb.current_seed();
    uint64_t mb_prey_seed = thread_sim.prey_mb.current_seed();

    thread_output << fmt::format(
        "generation {:>6}, predator {:>6}, seed {:>16}, fitness {:>9}, prey "
        "{:>6}, seed {:>16}, fitness {:>9}\n",
        generation, j, mb_pred_seed, pred_fitness_val, j, mb_prey_seed,
        prey_fitness_val);

    pred_seeds_with_fitness.emplace(mb_pred_seed, pred_fitness_val);
    prey_seeds_with_fitness.emplace(mb_prey_seed, prey_fitness_val);
  }

  return SimResult{pred_seeds_with_fitness, prey_seeds_with_fitness,
                   thread_output.str()};
}

void EvOverseer::sim() {
  using fit_seed_map = std::unordered_map<uint64_t, uint32_t>;
  using task_type = OptSimResult(uint32_t, uint32_t, std::vector<MarkovBrain>&,
                                 std::vector<MarkovBrain>&);

  using namespace std::placeholders;

  std::random_device rd;
  pcg64 gen(rd());

  std::ofstream evolved_mb_file;
  std::ofstream fitness_file("fitness.txt");

  std::vector<std::future<OptSimResult>> futures;
  std::vector<std::packaged_task<task_type>> tasks;
  std::vector<std::thread> workers;

  fit_seed_map pred_seeds_with_fitness;
  fit_seed_map prey_seeds_with_fitness;

  const auto& viewport = *_settings.get("viewport")->as_table();
  const auto& simulation = *_settings.get("simulation")->as_table();

  const auto universe_width =
      simulation["universe"]["width"].as_integer()->get();
  const auto universe_height =
      simulation["universe"]["height"].as_integer()->get();

  if (!viewport["headless"].as_boolean()->get() &&
      simulation["threads"].as_integer()->get() == 1) {
    const auto w_scale =
        static_cast<double>(viewport["width"].as_integer()->get()) /
        universe_width;
    const auto h_scale =
        static_cast<double>(viewport["height"].as_integer()->get()) /
        universe_height;

    _view = std::make_unique<MainView>(viewport["width"].as_integer()->get(),
                                       viewport["height"].as_integer()->get(),
                                       w_scale, h_scale);
  }

  uint32_t generations =
      _settings["simulation"]["generations"].as_integer()->get();

  uint32_t threads = _settings["simulation"]["threads"].as_integer()->get();

  double pred_fitness_geom_mean = 0;
  double prey_fitness_geom_mean = 0;

  for (uint32_t generation = 0; generation < generations; ++generation) {
    std::vector<MarkovBrain> pred_pool{_pred_mb_pool};
    std::vector<MarkovBrain> prey_pool{_prey_mb_pool};

    std::shuffle(std::begin(pred_pool), std::end(pred_pool), gen);
    std::shuffle(std::begin(prey_pool), std::end(prey_pool), gen);

    fmt::print("Generation {}/{}:\n", generation + 1, generations);

    for (uint32_t t = 0; t < threads; ++t) {
      tasks.emplace_back(std::packaged_task<task_type>(
          std::bind(&EvOverseer::_run_thread, this, _1, _2, _3, _4)));
      futures.emplace_back(tasks[t].get_future());
    }

    {
      using namespace std::chrono;

      steady_clock::time_point start, end;

      start = steady_clock::now();

      // call to parallel code here
      for (uint32_t t = 0; t < threads; ++t) {
        workers.emplace_back(std::thread(std::move(tasks[t]), t, generation,
                                         std::ref(pred_pool),
                                         std::ref(prey_pool)));
      }

      for (auto& thread : workers) {
        thread.join();
      }
      end = steady_clock::now();

      for (auto& future : futures) {
        if (auto result = future.get()) {
          auto [pred_seeds_fit, prey_seeds_fit, sim_output] = *result;

          pred_seeds_with_fitness.insert(std::begin(pred_seeds_fit),
                                         std::end(pred_seeds_fit));
          prey_seeds_with_fitness.insert(std::begin(prey_seeds_fit),
                                         std::end(prey_seeds_fit));

          fitness_file << sim_output;
        } else {
          fmt::print("User interrupted simulation\n");
          fitness_file.close();
          return;
        }
      }

      // Increase mutation rate depending on the fitness
      // TODO: per agent mutation rate

      if (_settings["simulation"]["evolve predator"].as_boolean()->get()) {
        double tmp_pred_fit_geom_mean = 1;
        for (const auto [_, fitness] : pred_seeds_with_fitness) {
          tmp_pred_fit_geom_mean *= fitness > 0 ? fitness : 1;
        }
        tmp_pred_fit_geom_mean = std::pow(tmp_pred_fit_geom_mean,
                                          1.0 / pred_seeds_with_fitness.size());

        if (tmp_pred_fit_geom_mean < pred_fitness_geom_mean * 1.01) {
          MarkovBrain::increase_mutation_rate();
        } else {
          MarkovBrain::decrease_mutation_rate();
        }
        pred_fitness_geom_mean = tmp_pred_fit_geom_mean;
      }
      if (_settings["simulation"]["evolve prey"].as_boolean()->get()) {
        double tmp_prey_fit_geom_mean = 1;
        for (const auto [_, fitness] : pred_seeds_with_fitness) {
          tmp_prey_fit_geom_mean *= fitness > 0 ? fitness : 1;
        }
        tmp_prey_fit_geom_mean = std::pow(tmp_prey_fit_geom_mean,
                                          1.0 / pred_seeds_with_fitness.size());

        if (tmp_prey_fit_geom_mean < prey_fitness_geom_mean * 1.01) {
          MarkovBrain::increase_mutation_rate();
        } else {
          MarkovBrain::decrease_mutation_rate();
        }
        prey_fitness_geom_mean = tmp_prey_fit_geom_mean;
      }

      fmt::print(
          "  - duration: {}s\n"
          "  - predator mean (geom) fitness: {}\n"
          "  - prey mean (geom) fitness:     {}\n"
          "  - mutation rate: {}\n",
          duration_cast<seconds>(end - start).count(), pred_fitness_geom_mean,
          prey_fitness_geom_mean, MarkovBrain::get_mutation_rate());
    }
    if (_settings["simulation"]["evolve predator"].as_boolean()->get()) {
      const auto pred_mb_dir = fmt::format("pred_mb/{}", generation);
      std::filesystem::create_directories(pred_mb_dir);

      for (auto& predator_mb : _pred_mb_pool) {
        const auto seed = predator_mb.current_seed();
        const auto fitness = pred_seeds_with_fitness[seed];

        evolved_mb_file.open(
            fmt::format("{}/{}_{}.txt", pred_mb_dir, fitness, seed),
            std::ofstream::out);
        evolved_mb_file << predator_mb;
        evolved_mb_file.close();
      }
      _moran_process(pred_seeds_with_fitness, _pred_mb_pool);
    }
    if (_settings["simulation"]["evolve prey"].as_boolean()->get()) {
      const auto prey_mb_dir = fmt::format("prey_mb/{}", generation);
      std::filesystem::create_directories(prey_mb_dir);

      for (auto& prey_mb : _prey_mb_pool) {
        const auto seed = prey_mb.current_seed();
        const auto fitness = prey_seeds_with_fitness[seed];

        evolved_mb_file.open(
            fmt::format("{}/{}_{}.txt", prey_mb_dir, fitness, seed),
            std::ofstream::out);
        evolved_mb_file << prey_mb;
        evolved_mb_file.close();
      }
      _moran_process(prey_seeds_with_fitness, _prey_mb_pool);
    }

    pred_seeds_with_fitness.clear();
    prey_seeds_with_fitness.clear();
    workers.clear();
    tasks.clear();
    futures.clear();
  }

  fitness_file.close();
}

uint64_t EvOverseer::_stochastic_acceptance(
    std::unordered_map<uint64_t, uint32_t> seeds_with_fitness) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> select_proba_dist(0, 1);
  std::uniform_int_distribution<uint32_t> rand_index_dist(
      0, seeds_with_fitness.size() - 1);

  std::pair<uint64_t, uint32_t> selected_individual;
  uint32_t total_generation_fitness = 0;

  bool selection_done = false;

  for (auto const& seed_with_fit : seeds_with_fitness) {
    total_generation_fitness += seed_with_fit.second;
  }

  while (!selection_done) {
    selected_individual =
        *std::next(std::begin(seeds_with_fitness), rand_index_dist(gen));

    auto selection_proba = static_cast<double>(selected_individual.second) /
                           total_generation_fitness;
    if (selection_proba > select_proba_dist(gen)) {
      selection_done = true;
    }
  }

  return selected_individual.first;
}

void EvOverseer::_moran_process(
    std::unordered_map<uint64_t, uint32_t> const& mb_seeds_fit,
    std::vector<MarkovBrain>& population) {
  std::vector<MarkovBrain> offsprings;
  const auto& mutations_probas = _settings.get("genome mutation")->as_table();

  for (uint32_t i = 0; i < population.size(); ++i) {
    uint64_t parent_mb_seed = _stochastic_acceptance(mb_seeds_fit);
    auto parent_mb = *std::find_if(std::begin(population), std::end(population),
                                   [parent_mb_seed](MarkovBrain const& mb) {
                                     return mb.current_seed() == parent_mb_seed;
                                   });

    offsprings.push_back(parent_mb);
  }

  population.clear();
  population = std::move(offsprings);

  for (auto& evolving_mb : population) {
    evolving_mb.mutation(*mutations_probas);
  }
}

std::ostream& ::sim::operator<<(std::ostream& os, EvOverseer const& lightsim) {
  os << lightsim._settings << std::endl << std::endl;

  return os;
}
