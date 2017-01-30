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
  uint32_t pred_generations = _settings["pred_generations"];
  uint32_t pred_children = _settings["pred_children"];
  uint32_t threads = _settings["threads"];
  uint32_t loop_range_begin;
  uint32_t loop_range_end;

  using task_type = std::map<uint32_t, uint64_t>(uint32_t, uint32_t, uint32_t);

  auto pred_child_eval{
      [this](uint32_t gen, uint32_t loop_range_begin, uint32_t loop_range_end) {
        std::map<uint32_t, uint64_t> fitness_with_seeds;
        LocalThreadSim thread_sim(_seed, _settings, _env.get(), _view.get(),
                                  this->_pred_mb_init, this->_prey_mb_init);

        for (uint32_t j = loop_range_begin; j < loop_range_end; ++j) {
          thread_sim.pred_mb.gaussian_mutation();
          thread_sim.run();

          auto fitness_val = thread_sim.eval_pred();

          this->_io_mutex.lock();
          std::cout << "Gen: " << gen << " child: " << j
                    << " fitness: " << fitness_val << std::endl;
          this->_io_mutex.unlock();

          fitness_with_seeds[fitness_val] = thread_sim.pred_mb.current_seed();
          thread_sim.pred_mb = this->_pred_mb_init;
        }
        return fitness_with_seeds;
      }};

  std::vector<std::future<std::map<uint32_t, uint64_t>>> futures;
  std::vector<std::packaged_task<task_type>> tasks;
  std::vector<std::thread> pred_workers;

  for (uint32_t i = 0; i < pred_generations; ++i) {
    mb_file.open("Predator/pred_0.bin", std::ios::binary | std::ios::out);
    mb_file << _pred_mb_init;
    mb_file.close();

    // init tasks
    for (uint32_t t = 0; t < threads; ++t) {
      tasks.emplace_back(std::packaged_task<task_type>{pred_child_eval});
      futures.emplace_back(tasks[t].get_future());
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

    auto it_best_child = fitness_with_seeds.rbegin();

    auto best_child_1 = *it_best_child;
    auto best_child_2 = *(++it_best_child);

    _pred_mb_init.gaussian_mutation(best_child_1.second);
    mb_file.open("Predator/best_mb_1.bin", std::ios::out | std::ios::binary);
    mb_file << _pred_mb_init;
    mb_file.close();

    mb_file.open("Predator/pred_0.bin", std::ios::in | std::ios::binary);
    mb_file >> _pred_mb_init;
    mb_file.close();

    mb_file.open("Predator/best_mb_1.bin", std::ios::in | std::ios::binary);
    _pred_mb_init.gaussian_mutation(best_child_2.second);
    _pred_mb_init.crossover(mb_file);
    mb_file.close();

    _pred_mb_init.self_adaptation();

    fitness_with_seeds.clear();
    pred_workers.clear();
    tasks.clear();
    futures.clear();
  }
}

void LightSim::_setup_sim() {
  auto& set = _settings;
  double w_scale = static_cast<double>(set["win_w"]) / set["grid_w"];
  double h_scale = static_cast<double>(set["win_h"]) / set["grid_h"];

  _env.reset(new Environment(set["grid_w"], set["grid_h"]));

  _view.reset(nullptr);

  //  _view.reset(set["headless"] == 0 ? (new MainView(set["win_w"],
  //  set["win_h"],
  //                                                   w_scale, h_scale,
  //                                                   _agents))
  //                                   : nullptr);

  _prey_mb_init = MarkovBrain2(set["prey_retina_cells_by_layer"] * 2, 2, true);
  _pred_mb_init = MarkovBrain2(set["pred_retina_cells"], 2, true);
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
