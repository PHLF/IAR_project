#include <Controller/LightSim.h>

using namespace sim;

LightSim::LightSim() {
  std::random_device rd;
  _seed = rd();
}

LightSim::~LightSim() {}

void LightSim::sim() {
  bool evo_pred = _settings["evolve_pred"] == 1;
  bool evo_prey = _settings["evolve_prey"] == 1;

  if (evo_pred) {
    _train_predator();
  }
  if (evo_prey) {
    //_train_prey();
  }
  if (!evo_pred && !evo_prey) {
    //    _run();
  }
}

void LightSim::test_pred(std::istream& is) {
  is >> _pred_mb_init;
  //  _run();
}

void LightSim::_train_predator() {
  std::map<uint32_t, uint64_t> fitness_with_seeds;
  std::stringstream filename;
  std::fstream mb_file;
  std::ofstream fitness_file("predator_fitness.txt");
  std::array<uint64_t, 2> parents;
  uint32_t pred_generations = _settings["pred_generations"];
  uint32_t pred_children = _settings["pred_children"];
  uint32_t threads = _settings["threads"];
  uint32_t loop_range_begin;
  uint32_t loop_range_end;

  using task_type = std::map<uint32_t, uint64_t>(uint32_t, uint32_t, uint32_t);

  auto first_gen_eval{[this, &fitness_file](
      uint32_t gen, uint32_t loop_range_begin, uint32_t loop_range_end) {
    std::map<uint32_t, uint64_t> fitness_with_seeds;
    LocalThreadSim thread_sim(_seed, _settings, _io_mutex, _pred_mb_init,
                              _prey_mb_init);

    for (uint32_t j = loop_range_begin; j < loop_range_end; ++j) {
      thread_sim.pred_mb.random_fill();
      thread_sim.run();

      auto fitness_val = thread_sim.eval_pred();

      _io_mutex.lock();
      fitness_file << "generation_" << gen << "_predator_" << j << " "
                   << fitness_val << std::endl;
      _io_mutex.unlock();

      fitness_with_seeds[fitness_val] = thread_sim.pred_mb.current_seed();
    }
    return fitness_with_seeds;
  }};

  auto pred_child_eval{[this, &fitness_file](
      uint32_t gen, uint32_t loop_range_begin, uint32_t loop_range_end) {
    std::map<uint32_t, uint64_t> fitness_with_seeds;
    LocalThreadSim thread_sim(_seed, _settings, _io_mutex, _pred_mb_init,
                              _prey_mb_init);

    for (uint32_t j = loop_range_begin; j < loop_range_end; ++j) {
      thread_sim.pred_mb.gaussian_mutation();
      thread_sim.run();

      auto fitness_val = thread_sim.eval_pred();

      _io_mutex.lock();
      fitness_file << "generation_" << gen << "_predator_" << j << " "
                   << fitness_val << std::endl;
      _io_mutex.unlock();

      fitness_with_seeds[fitness_val] = thread_sim.pred_mb.current_seed();
      thread_sim.pred_mb = this->_pred_mb_init;
    }
    return fitness_with_seeds;
  }};

  std::vector<std::future<std::map<uint32_t, uint64_t>>> futures;
  std::vector<std::packaged_task<task_type>> tasks;
  std::vector<std::thread> pred_workers;

  for (uint32_t i = 0; i < pred_generations; ++i) {
    std::cout << "SIGMA: " << _pred_mb_init.sigma << std::endl;
    mb_file.open("Predator/pred_0.bin", std::ios::binary | std::ios::out);
    mb_file << _pred_mb_init;
    mb_file.close();

    // init tasks
    if (i == 0) {
      for (uint32_t t = 0; t < threads; ++t) {
        tasks.emplace_back(std::packaged_task<task_type>{first_gen_eval});
        futures.emplace_back(tasks[t].get_future());
      }
    } else {
      for (uint32_t t = 0; t < threads; ++t) {
        tasks.emplace_back(std::packaged_task<task_type>{pred_child_eval});
        futures.emplace_back(tasks[t].get_future());
      }
    }
    // call to parallel code here
    for (uint32_t t = 0; t < threads; ++t) {
      loop_range_begin = t * (pred_children / threads);
      loop_range_end = (t + 1) * (pred_children / threads);
      pred_workers.emplace_back(std::thread{std::move(tasks[t]), i,
                                            loop_range_begin, loop_range_end});
    }

    for (auto& thread : pred_workers) {
      thread.join();
    }

    for (auto& future : futures) {
      auto fitness_with_seed = future.get();
      fitness_with_seeds.insert(fitness_with_seed.begin(),
                                fitness_with_seed.end());
    }

    parents = _select_parents(fitness_with_seeds);

    if (i == 0) {
      _pred_mb_init.random_fill(parents[0]);
      mb_file.open("Predator/parent_1.bin", std::ios::out | std::ios::binary);
      mb_file << _pred_mb_init;
      mb_file.close();

      mb_file.open("Predator/parent_1.bin", std::ios::in | std::ios::binary);
      _pred_mb_init.random_fill(parents[1]);
      _pred_mb_init.crossover(mb_file);
      mb_file.close();
    } else {
      _pred_mb_init.gaussian_mutation(parents[0]);
      mb_file.open("Predator/parent_1.bin", std::ios::out | std::ios::binary);
      mb_file << _pred_mb_init;
      mb_file.close();

      mb_file.open("Predator/pred_0.bin", std::ios::in | std::ios::binary);
      mb_file >> _pred_mb_init;
      mb_file.close();

      mb_file.open("Predator/parent_1.bin", std::ios::in | std::ios::binary);
      _pred_mb_init.gaussian_mutation(parents[1]);
      _pred_mb_init.crossover(mb_file);
      mb_file.close();
    }

    _pred_mb_init.self_adaptation();

    fitness_with_seeds.clear();
    pred_workers.clear();
    tasks.clear();
    futures.clear();
  }
  fitness_file.close();
}

std::array<uint64_t, 2> LightSim::_select_parents(
    std::map<uint32_t, uint64_t>& fitness_with_seeds) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> uni_d(0, 1);

  std::array<uint64_t, 2> parents;

  uint32_t total_generation_fitness = 0;
  double rnd_select = 0;
  double selection_probability = 0;

  bool chosen_parent_1 = false;
  bool chosen_parent_2 = false;

  for (auto const& fit_with_seed : fitness_with_seeds) {
    total_generation_fitness += fit_with_seed.first;
  }

  while (!(chosen_parent_1 && chosen_parent_2)) {
    rnd_select = uni_d(gen);
    auto it_pred = fitness_with_seeds.begin();

    selection_probability = 0;
    while (it_pred != fitness_with_seeds.end() && !chosen_parent_1) {
      selection_probability +=
          static_cast<double>(it_pred->first) / total_generation_fitness;
      if (selection_probability > rnd_select) {
        parents[0] = it_pred->second;
        fitness_with_seeds.erase(it_pred);
        chosen_parent_1 = true;
        break;
      }
      ++it_pred;
    }

    selection_probability = 0;
    it_pred = fitness_with_seeds.begin();
    while (it_pred != fitness_with_seeds.end() && !chosen_parent_2) {
      selection_probability +=
          static_cast<double>(it_pred->first) / total_generation_fitness;
      if (selection_probability > rnd_select) {
        parents[1] = it_pred->second;
        fitness_with_seeds.erase(it_pred);
        chosen_parent_2 = true;
        break;
      }
      ++it_pred;
    }
  }

  return parents;
}

void LightSim::_setup_sim() {
  auto& set = _settings;

  _prey_mb_init = MarkovBrain(set["prey_retina_cells_by_layer"] * 2, 2, true);
  _pred_mb_init = MarkovBrain(set["pred_retina_cells"], 2, true);
}

std::ostream& ::sim::operator<<(std::ostream& os, LightSim const& lightsim) {
  for (auto const& pair : lightsim._settings) {
    os << pair.first << " " << pair.second << std::endl;
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
