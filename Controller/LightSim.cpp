#include "LightSim.h"

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
    if (!(set["predator_file_fitness_value"] == 0 ||
          set["prey_file_fitness_value"] == 0)) {
      MarkovBrain pred_mb, prey_mb;
      std::stringstream filename;

      filename << "pred_mb_" << set["predator_file_fitness_value"] << ".txt";
      auto a = filename.str();
      myfile.open(filename.str(), std::ios::in);
      myfile >> pred_mb;
      myfile.close();
      filename.str("");
      filename.clear();

      filename << "prey_mb_" << set["prey_file_fitness_value"] << ".txt";

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
                                          std::vector<MarkovBrain>& pred_pool,
                                          std::vector<MarkovBrain>& prey_pool) {
  uint32_t loop_range_begin = 0;
  uint32_t loop_range_end = 0;

  std::stringstream thread_output;

  std::map<uint32_t, uint64_t> pred_fitness_with_seeds;
  std::map<uint32_t, uint64_t> prey_fitness_with_seeds;

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

  auto [pred_mb0, prey_mb0] = get_mbs_pair(local_pred_pool, local_prey_pool);

  LocalThreadSim thread_sim(_settings, pred_mb0, prey_mb0);

  for (uint32_t j = loop_range_begin; j < loop_range_end; ++j) {
    if (j != loop_range_begin) {
      auto [pred_mb, prey_mb] = get_mbs_pair(local_pred_pool, local_prey_pool);

      thread_sim.pred_mb = std::move(pred_mb);
      thread_sim.prey_mb = std::move(prey_mb);
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

  return SimResult{pred_fitness_with_seeds, prey_fitness_with_seeds,
                   thread_output.str()};
}

void LightSim::sim() {
  using fit_seed_map = std::map<uint32_t, uint64_t>;
  using task_type =
      SimResult(uint32_t, std::vector<MarkovBrain>&, std::vector<MarkovBrain>&);

  using namespace std::placeholders;

  std::random_device rd;
  std::mt19937 gen(rd());

  std::ofstream best_pred_file;
  std::ofstream best_prey_file;
  std::ofstream fitness_file("predator_fitness.txt");

  std::vector<std::future<SimResult>> futures;
  std::vector<std::packaged_task<task_type>> tasks;
  std::vector<std::thread> workers;

  fit_seed_map pred_fitness_with_seeds;
  fit_seed_map prey_fitness_with_seeds;

  uint32_t generations = _settings["generations"];

  uint32_t threads = _settings["threads"];

  for (uint32_t i = 0; i < generations; ++i) {
    std::vector<MarkovBrain> pred_pool{_pred_pool};
    std::vector<MarkovBrain> prey_pool{_prey_pool};
    std::shuffle(std::begin(pred_pool), std::end(pred_pool), gen);
    std::shuffle(std::begin(prey_pool), std::end(prey_pool), gen);

    for (uint32_t t = 0; t < threads; ++t) {
      tasks.emplace_back(std::packaged_task<task_type>(
          std::bind(&LightSim::_run_thread, this, _1, _2, _3)));
      futures.emplace_back(tasks[t].get_future());
    }

    // call to parallel code here
    for (uint32_t t = 0; t < threads; ++t) {
      workers.emplace_back(std::thread(
          std::move(tasks[t]), t, std::ref(pred_pool), std::ref(prey_pool)));
    }

    for (auto& thread : workers) {
      thread.join();
    }

    fitness_file << "generation " << i << std::endl;
    for (auto& future : futures) {
      auto [pred_fit_seeds, prey_fit_seeds, sim_output] = future.get();

      pred_fitness_with_seeds.insert(std::begin(pred_fit_seeds),
                                     std::end(pred_fit_seeds));
      prey_fitness_with_seeds.insert(std::begin(prey_fit_seeds),
                                     std::end(prey_fit_seeds));

      fitness_file << sim_output;
    }

    if (_settings["evolve_pred"] == 1) {
      std::stringstream filename;
      auto [selected_mb_fitness_val, mb_as_string] =
          _moran_process(pred_fitness_with_seeds, _pred_pool);

      filename << "pred_mb_" << selected_mb_fitness_val << ".txt";

      best_pred_file.open(filename.str(), std::ofstream::out);
      best_pred_file << mb_as_string;
      best_pred_file.close();
    }
    if (_settings["evolve_prey"] == 1) {
      std::stringstream filename;
      auto [selected_mb_fitness_val, mb_as_string] =
          _moran_process(prey_fitness_with_seeds, _prey_pool);

      filename << "prey_mb_" << selected_mb_fitness_val << ".txt";

      best_prey_file.open(filename.str(), std::ofstream::out);
      best_prey_file << mb_as_string;
      best_prey_file.close();
    }

    pred_fitness_with_seeds.clear();
    prey_fitness_with_seeds.clear();
    workers.clear();
    tasks.clear();
    futures.clear();
  }
  fitness_file.close();
}

std::tuple<uint64_t, uint32_t> LightSim::_fitness_proportionate_selection(
    std::map<uint32_t, uint64_t> fitness_with_seeds,
    bool unfitness) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> uni_d(0, 1);

  uint64_t chosen_individual = 0;
  uint32_t chosen_individual_fitness = 0;
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
      if (unfitness ? (1 - selection_probability) > rnd_select
                    : selection_probability > rnd_select) {
        chosen_individual = it_pred->second;
        chosen_individual_fitness = it_pred->first;
        fitness_with_seeds.erase(it_pred);
        done_selecting = true;
        break;
      }
      ++it_pred;
    }
  }

  return {chosen_individual, chosen_individual_fitness};
}

std::tuple<uint32_t, std::string> LightSim::_moran_process(
    std::map<uint32_t, uint64_t> const& mb_fit_seeds,
    std::vector<MarkovBrain>& population) {
  std::stringstream os;
  std::unordered_map<std::string, uint32_t> mutations_probas;
  for (auto const& [ key, value ] : _settings) {
    if (key.substr(0, 6) == "proba_") {
      mutations_probas[key] = value;
    }
  }

  auto [cloning_mb_seed, cloning_mb_fitness] =
      _fitness_proportionate_selection(mb_fit_seeds);
  auto reproducing_mb =
      std::find_if(std::begin(population), std::end(population),
                   [cloning_mb_seed](MarkovBrain const& mb) {
                     bool test = mb.current_seed() == cloning_mb_seed;
                     return test;
                   });

  MarkovBrain cloned_mb;
  cloned_mb = *reproducing_mb;
  cloned_mb.mutation(mutations_probas);

  population.push_back(cloned_mb);

  auto [mb_seed_to_delete, dummy] =
      _fitness_proportionate_selection(mb_fit_seeds, true);
  population.erase(std::remove_if(std::begin(population), std::end(population),
                                  [mb_seed_to_delete](MarkovBrain const& mb) {
                                    bool test =
                                        mb.current_seed() == mb_seed_to_delete;
                                    return test;
                                  }));
  os << cloned_mb;
  return {cloning_mb_fitness, os.str()};
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
