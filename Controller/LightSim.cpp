#include "LightSim.h"

using namespace sim;

LightSim::LightSim() {}

LightSim::~LightSim() {}

void LightSim::sim() {
  using fit_seed_map = std::map<uint32_t, uint64_t>;
  using sim_result_type = std::tuple<fit_seed_map, fit_seed_map, std::string>;
  using task_type = sim_result_type(uint32_t, std::vector<MarkovBrain>&,
                                    std::vector<MarkovBrain>&);

  std::random_device rd;
  std::mt19937 gen(rd());

  std::ofstream fitness_file("predator_fitness.txt");

  std::vector<std::future<sim_result_type>> futures;
  std::vector<std::packaged_task<task_type>> tasks;
  std::vector<std::thread> workers;

  fit_seed_map pred_fitness_with_seeds;
  fit_seed_map prey_fitness_with_seeds;

  uint32_t generations = _settings["pred_generations"];

  uint32_t threads = _settings["threads"];

  auto run = [&](uint32_t thread_number, std::vector<MarkovBrain>& pred_pool,
                 std::vector<MarkovBrain>& prey_pool) {

    uint32_t loop_range_begin = 0;
    uint32_t loop_range_end = 0;

    std::stringstream thread_output;

    std::map<uint32_t, uint64_t> pred_fitness_with_seeds;
    std::map<uint32_t, uint64_t> prey_fitness_with_seeds;

    uint32_t pred_pool_size = static_cast<uint32_t>(pred_pool.size());

    loop_range_begin = thread_number * (pred_pool_size / threads);
    loop_range_end = (thread_number + 1) * (pred_pool_size / threads);

    std::vector<MarkovBrain> local_pred_pool{
        std::begin(pred_pool) + loop_range_begin,
        std::end(pred_pool) + loop_range_end};
    std::vector<MarkovBrain> local_prey_pool{
        std::begin(prey_pool) + loop_range_begin,
        std::end(prey_pool) + loop_range_end};

    auto get_mbs_pair = [](std::vector<MarkovBrain>& pred_pool,
                           std::vector<MarkovBrain>& prey_pool) {

      auto pred_mb = pred_pool.back();
      pred_pool.pop_back();

      auto prey_mb = prey_pool.back();
      prey_pool.pop_back();

      return std::make_pair(pred_mb, prey_mb);
    };

    auto mbs_pair = get_mbs_pair(local_pred_pool, local_prey_pool);

    LocalThreadSim thread_sim(_settings, std::get<0>(mbs_pair),
                              std::get<1>(mbs_pair));

    for (uint32_t j = loop_range_begin; j < loop_range_end; ++j) {
      if (j != loop_range_begin) {
        auto mbs_pair = get_mbs_pair(local_pred_pool, local_prey_pool);

        thread_sim.pred_mb = std::move(std::get<0>(mbs_pair));
        thread_sim.prey_mb = std::move(std::get<1>(mbs_pair));
      }

      thread_sim.run();

      uint32_t pred_fitness_val = thread_sim.eval_pred();
      uint32_t prey_fitness_val = thread_sim.eval_prey();

      thread_output << "predator_" << j << " " << pred_fitness_val << " "
                    << "prey_" << j << " " << prey_fitness_val << std::endl;

      pred_fitness_with_seeds[pred_fitness_val] =
          thread_sim.pred_mb.current_seed();
      prey_fitness_with_seeds[prey_fitness_val] =
          thread_sim.prey_mb.current_seed();
    }

    return std::make_tuple(pred_fitness_with_seeds, prey_fitness_with_seeds,
                           thread_output.str());
  };

  for (uint32_t i = 0; i < generations; ++i) {
    std::vector<MarkovBrain> pred_pool{_pred_pool};
    std::vector<MarkovBrain> prey_pool{_prey_pool};

    std::shuffle(std::begin(pred_pool), std::end(pred_pool), gen);
    std::shuffle(std::begin(prey_pool), std::end(prey_pool), gen);

    for (uint32_t t = 0; t < threads; ++t) {
      tasks.emplace_back(std::packaged_task<task_type>{run});
      futures.emplace_back(tasks[t].get_future());
    }

    // call to parallel code here
    for (uint32_t t = 0; t < threads; ++t) {
      workers.emplace_back(
          std::thread{std::move(tasks[t]), t, pred_pool, prey_pool});
    }

    for (auto& thread : workers) {
      thread.join();
    }

    fitness_file << "generation " << i << std::endl;
    for (auto& future : futures) {
      auto sim_result = future.get();
      auto pred_fitness_with_seeds = std::get<0>(sim_result);
      auto prey_fitness_with_seeds = std::get<1>(sim_result);
      auto thread_output = std::get<2>(sim_result);

      pred_fitness_with_seeds.insert(pred_fitness_with_seeds.begin(),
                                     pred_fitness_with_seeds.end());
      prey_fitness_with_seeds.insert(prey_fitness_with_seeds.begin(),
                                     prey_fitness_with_seeds.end());

      fitness_file << thread_output;
    }

    if (_settings["evolve_pred"] == 1) {
    }
    if (_settings["evolve_prey"] == 1) {
    }

    pred_fitness_with_seeds.clear();
    workers.clear();
    tasks.clear();
    futures.clear();
  }
  fitness_file.close();
}

uint64_t LightSim::_fitness_proportionate_selection(
    std::map<uint32_t, uint64_t>& fitness_with_seeds) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> uni_d(0, 1);

  uint64_t chosen_individual = 0;
  uint32_t total_generation_fitness = 0;
  double rnd_select = 0;
  double selection_probability = 0;

  bool done_selecting = false;

  for (auto const& fit_with_seed : fitness_with_seeds) {
    total_generation_fitness += fit_with_seed.first;
  }

  while (!done_selecting) {
    rnd_select = uni_d(gen);
    auto it_pred = fitness_with_seeds.begin();

    selection_probability = 0;
    while (it_pred != fitness_with_seeds.end() && !done_selecting) {
      selection_probability +=
          static_cast<double>(it_pred->first) / total_generation_fitness;
      if (selection_probability > rnd_select) {
        chosen_individual = it_pred->second;
        fitness_with_seeds.erase(it_pred);
        done_selecting = true;
        break;
      }
      ++it_pred;
    }
  }

  return chosen_individual;
}

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
    //    if (!(set["predator_file"].empty() || set["prey_file"].empty())) {
    //            myfile.open(set["predator_file"], std::ios::in);
    //            myfile >> _pred_mb_init;
    //            myfile.close();

    //            myfile.open(set["prey_file"], std::ios::in);
    //            myfile >> _pred_mb_init;
    //            myfile.close();
    //    }
  } else {
    for (uint32_t i = 0; i < set["pred_pool_size"]; ++i) {
      _pred_pool.emplace_back(
          MarkovBrain(pred_mb_max_inputs, pred_mb_max_outputs,
                      nb_nodes_for_predators, pred_mb_nb_ancestor_genes));
    }
    for (uint32_t i = 0; i < set["pred_pool_size"]; ++i) {
      _prey_pool.emplace_back(
          MarkovBrain(prey_mb_max_inputs, prey_mb_max_outputs,
                      nb_nodes_for_preys, prey_mb_nb_ancestor_genes));
    }
  }
}

std::ostream& sim::operator<<(std::ostream& os, LightSim const& lightsim) {
  for (auto const& pair : lightsim._settings) {
    os << pair.first << " " << pair.second << std::endl;
  }
  return os;
}

std::istream& sim::operator>>(std::istream& is, LightSim& lightsim) {
  std::string key;
  uint32_t val;
  while (is) {
    is >> std::skipws >> key >> val;
    lightsim._settings[key] = val;
  }

  lightsim._setup_sim();
  return is;
}
