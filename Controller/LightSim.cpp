﻿#include "LightSim.h"

using namespace sim;

LightSim::LightSim() {}

LightSim::~LightSim() {}

void LightSim::_setup_sim() {
  std::ifstream myfile;
  auto& set = _settings;

  uint32_t const nb_nodes_for_predators = set["pred_retina_cells"] + 2;
  uint32_t const pred_mb_max_inputs = set["pred_mb_max_inputs"];
  uint32_t const pred_mb_max_outputs = set["pred_mb_max_outputs"];
  uint32_t const pred_mb_nb_ancestor_genes = set["pred_mb_nb_ancestor_genes"];

  uint32_t const nb_nodes_for_preys = set["prey_retina_cells_by_layer"] * 2 + 2;
  uint32_t const prey_mb_max_inputs = set["prey_mb_max_inputs"];
  uint32_t const prey_mb_max_outputs = set["prey_mb_max_outputs"];
  uint32_t const prey_mb_nb_ancestor_genes = set["prey_mb_nb_ancestor_genes"];

  if ((set["evolve_pred"] == 0 && set["evolve_prey"] == 0)) {
    if (!(set["predator_file_seed_value"] == 0 ||
          set["prey_file_seed_value"] == 0)) {
      MarkovBrain pred_mb, prey_mb;
      std::stringstream filename;

      filename << "pred_mb_" << set["predator_file_seed_value"] << ".txt";
      auto a = filename.str();
      myfile.open(filename.str(), std::ios::in);
      myfile >> pred_mb;
      myfile.close();
      filename.str("");
      filename.clear();

      filename << "prey_mb_" << set["prey_file_seed_value"] << ".txt";

      myfile.open(filename.str(), std::ios::in);
      myfile >> prey_mb;
      myfile.close();

      _pred_pool.push_back(pred_mb);
      _prey_pool.push_back(prey_mb);
    }
  } else {
    for (uint32_t i = 0; i < set["pool_size"]; ++i) {
      _pred_pool.emplace_back(
          MarkovBrain(pred_mb_max_inputs, pred_mb_max_outputs,
                      nb_nodes_for_predators, pred_mb_nb_ancestor_genes));

      _prey_pool.emplace_back(
          MarkovBrain(prey_mb_max_inputs, prey_mb_max_outputs,
                      nb_nodes_for_preys, prey_mb_nb_ancestor_genes));
    }
  }
}

LightSim::SimResult LightSim::_run_thread(uint32_t thread_number,
                                          uint32_t generation,
                                          std::vector<MarkovBrain>& pred_pool,
                                          std::vector<MarkovBrain>& prey_pool) {
  uint32_t loop_range_begin = 0;
  uint32_t loop_range_end = 0;

  std::stringstream thread_output;

  std::unordered_map<uint64_t, uint32_t> pred_seeds_with_fitness;
  std::unordered_map<uint64_t, uint32_t> prey_seeds_with_fitness;

  uint32_t pool_size = static_cast<uint32_t>(pred_pool.size());
  uint32_t threads = _settings["threads"];

  loop_range_begin = thread_number * (pool_size / threads);
  loop_range_end = (thread_number + 1) * (pool_size / threads);

  std::vector<MarkovBrain> local_pred_pool{
      std::begin(pred_pool) + loop_range_begin,
      std::begin(pred_pool) + loop_range_end};
  std::vector<MarkovBrain> local_prey_pool{
      std::begin(prey_pool) + loop_range_begin,
      std::begin(prey_pool) + loop_range_end};

  auto get_mbs_pair = [](std::vector<MarkovBrain>& pred_pool_,
                         std::vector<MarkovBrain>& prey_pool_) {

    auto pred_mb = pred_pool_.back();
    pred_pool_.pop_back();

    auto prey_mb = prey_pool_.back();
    prey_pool_.pop_back();

    return std::make_pair(pred_mb, prey_mb);
  };

  auto[pred_mb0, prey_mb0] = get_mbs_pair(local_pred_pool, local_prey_pool);

  LocalThreadSim thread_sim(_settings, pred_mb0, prey_mb0);

  for (uint32_t j = loop_range_begin; j < loop_range_end; ++j) {
    if (j != loop_range_begin) {
      auto[pred_mb, prey_mb] = get_mbs_pair(local_pred_pool, local_prey_pool);

      thread_sim.pred_mb = std::move(pred_mb);
      thread_sim.prey_mb = std::move(prey_mb);
    }

    thread_sim.run();

    uint32_t pred_fitness_val = thread_sim.eval_pred();
    uint32_t prey_fitness_val = thread_sim.eval_prey();
    uint64_t mb_pred_seed = thread_sim.pred_mb.current_seed();
    uint64_t mb_prey_seed = thread_sim.prey_mb.current_seed();

    thread_output << "gen_" << generation << " "
                  << "predator_" << j << "_seed_" << mb_pred_seed << " "
                  << pred_fitness_val << " "
                  << "prey_" << j << "_seed_" << mb_prey_seed << " "
                  << prey_fitness_val << std::endl;

    pred_seeds_with_fitness[mb_pred_seed] = pred_fitness_val;
    prey_seeds_with_fitness[mb_prey_seed] = prey_fitness_val;
  }

  return SimResult{pred_seeds_with_fitness, prey_seeds_with_fitness,
                   thread_output.str()};
}

void LightSim::sim() {
  using fit_seed_map = std::unordered_map<uint64_t, uint32_t>;
  using task_type = SimResult(uint32_t, uint32_t, std::vector<MarkovBrain>&,
                              std::vector<MarkovBrain>&);

  using namespace std::placeholders;

  std::random_device rd;
  std::mt19937 gen(rd());

  std::ofstream best_pred_file;
  std::ofstream best_prey_file;
  std::ofstream fitness_file("fitness.txt");

  std::vector<std::future<SimResult>> futures;
  std::vector<std::packaged_task<task_type>> tasks;
  std::vector<std::thread> workers;

  fit_seed_map pred_seeds_with_fitness;
  fit_seed_map prey_seeds_with_fitness;

  uint32_t generations = _settings["generations"];

  uint32_t threads = _settings["threads"];

  for (uint32_t generation = 0; generation < generations; ++generation) {
    std::vector<MarkovBrain> pred_pool{_pred_pool};
    std::vector<MarkovBrain> prey_pool{_prey_pool};
    std::shuffle(std::begin(pred_pool), std::end(pred_pool), gen);
    std::shuffle(std::begin(prey_pool), std::end(prey_pool), gen);

    for (uint32_t t = 0; t < threads; ++t) {
      tasks.emplace_back(std::packaged_task<task_type>(
          std::bind(&LightSim::_run_thread, this, _1, _2, _3, _4)));
      futures.emplace_back(tasks[t].get_future());
    }

    // call to parallel code here
    for (uint32_t t = 0; t < threads; ++t) {
      workers.emplace_back(std::thread(std::move(tasks[t]), t, generation,
                                       std::ref(pred_pool),
                                       std::ref(prey_pool)));
    }

    for (auto& thread : workers) {
      thread.join();
    }

    for (auto& future : futures) {
      auto[pred_seeds_fit, prey_seeds_fit, sim_output] = future.get();

      pred_seeds_with_fitness.insert(std::begin(pred_seeds_fit),
                                     std::end(pred_seeds_fit));
      prey_seeds_with_fitness.insert(std::begin(prey_seeds_fit),
                                     std::end(prey_seeds_fit));

      fitness_file << sim_output;
    }

    if (_settings["evolve_pred"] == 1) {
      for (auto& predator_mb : _pred_pool) {
        std::stringstream filename;
        filename << "pred_mb_" << predator_mb.current_seed() << ".txt";

        best_pred_file.open(filename.str(), std::ofstream::out);
        best_pred_file << predator_mb;
        best_pred_file.close();
      }
      _moran_process(pred_seeds_with_fitness, _pred_pool);
    }
    if (_settings["evolve_prey"] == 1) {
      for (auto& prey_mb : _prey_pool) {
        std::stringstream filename;
        filename << "prey_mb_" << prey_mb.current_seed() << ".txt";

        best_pred_file.open(filename.str(), std::ofstream::out);
        best_pred_file << prey_mb;
        best_pred_file.close();
      }
      _moran_process(prey_seeds_with_fitness, _prey_pool);
    }

    pred_seeds_with_fitness.clear();
    prey_seeds_with_fitness.clear();
    workers.clear();
    tasks.clear();
    futures.clear();
  }

  fitness_file.close();
}

uint64_t LightSim::_stochastic_acceptance(
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

void LightSim::_moran_process(
    std::unordered_map<uint64_t, uint32_t> const& mb_seeds_fit,
    std::vector<MarkovBrain>& population) {
  std::unordered_map<std::string, uint32_t> mutations_probas;

  for (auto const & [ key, value ] : _settings) {
    if (key.substr(0, 6) == "proba_") {
      mutations_probas[key] = value;
    }
  }

  std::vector<MarkovBrain> offsprings;

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

  for (auto& agent : population) {
    agent.mutation(mutations_probas);
  }
}

std::ostream& ::sim::operator<<(std::ostream& os, LightSim const& lightsim) {
  for (auto const & [ key, value ] : lightsim._settings) {
    os << key << " " << value << std::endl;
  }
  return os;
}

std::istream& ::sim::operator>>(std::istream& is, LightSim& lightsim) {
  std::string key;
  uint32_t val;
  while (is) {
    is >> std::skipws >> key >> val;
    lightsim._settings[key] = val;
  }

  lightsim._setup_sim();
  return is;
}
