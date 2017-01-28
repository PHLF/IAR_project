﻿#include <Controller/LightSim.h>

using namespace sim;

LightSim::LightSim() {
  std::random_device rd;
  _seed = rd();
  _rd_gen.seed(_seed);
}

LightSim::~LightSim() {}

void LightSim::_print_agents() {
  for (auto const& agent : _agents) {
    std::cout << agent << std::endl;
  }
}

void LightSim::_move_agents() {
  std::vector<uint8_t> output;

  for (auto& agent : _agents) {
    agent.predates() ? output = pred_mb.actions(agent.get_state())
                     : output = prey_mb.actions(agent.get_state());

    if (output[0] != 0) {
      agent.turn_left();
    } else {
      agent.turned_left = false;
    }
    if (output[1] != 0) {
      agent.turn_right();
    } else {
      agent.turned_right = false;
    }

    auto temp_x = agent.coord.x + agent.speed * cos(agent.orientation);
    auto temp_y = agent.coord.y + agent.speed * sin(agent.orientation);
    /*
     * Torus map
        if (temp_x > _env->size_x) {
          temp_x = 0;
        } else if (temp_x < 0) {
          temp_x = _env->size_x;
        }
        if (temp_y > _env->size_y) {
          temp_y = 0;
        } else if (temp_y < 0) {
          temp_y = _env->size_y;
        }
        */
    if (temp_x > _env->size_x) {
      temp_x = _env->size_x;
    } else if (temp_x < 0) {
      temp_x = 0;
    }
    if (temp_y > _env->size_y) {
      temp_y = _env->size_y;
    } else if (temp_y < 0) {
      temp_y = 0;
    }

    agent.coord.x = temp_x;
    agent.coord.y = temp_y;
  }
}

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
    _run();
  }
}

void LightSim::_train_predator() {
  std::map<uint32_t, uint64_t> fitness_with_seeds;
  std::map<int, int> toto;
  std::stringstream filename;
  std::fstream mb_file;

  for (uint32_t i = 0; i < _settings["pred_generations"]; ++i) {
    mb_file.open("Predator/pred_0.bin", std::ios::binary | std::ios::out);
    mb_file << pred_mb;
    mb_file.close();

    for (uint32_t j = 0; j < _settings["pred_children"]; ++j) {
      mb_file.open("Predator/pred_0.bin", std::ios::binary | std::ios::in);

      pred_mb.gaussian_mutation();
      _run();

      auto fitness_val = eval_pred();
      std::cout << "Gen: " << i << " child: " << j
                << " fitness: " << fitness_val << std::endl;
      fitness_with_seeds[fitness_val] = pred_mb.current_seed();
      mb_file >> pred_mb;

      _reset_sim();

      mb_file.close();
    }

    auto it_best_child = fitness_with_seeds.rbegin();

    auto best_child_1 = *it_best_child;
    auto best_child_2 = *(++it_best_child);

    pred_mb.gaussian_mutation(best_child_1.second);
    mb_file.open("Predator/best_mb_1.bin", std::ios::out | std::ios::binary);
    mb_file << pred_mb;
    mb_file.close();

    mb_file.open("Predator/pred_0.bin", std::ios::binary | std::ios::in);
    mb_file >> pred_mb;
    mb_file.close();

    mb_file.open("Predator/best_mb_1.bin", std::ios::in | std::ios::binary);
    pred_mb.gaussian_mutation(best_child_2.second);
    pred_mb.crossover(mb_file);
    mb_file.close();

    pred_mb.self_adaptation();
  }
}

void LightSim::_capture_preys() {
  auto it_agent = _agents.begin();

  while (it_agent != _agents.end()) {
    Agent& agent = *it_agent;
    if (agent.predates()) {
      if (agent.handling_time == 0) {
        _agents.erase(
            std::remove_if(_agents.begin(), _agents.end(),
                           [&agent](Agent& x) {
                             bool prey = !x.predates();
                             bool cap = false;

                             if (prey) {
                               cap = static_cast<Predator&>(agent).captures(x);
                             }
                             return cap;
                           }),
            _agents.end());
      } else if (agent.handling_time > 0) {
        agent.handling_time--;
      }
    }

    ++it_agent;
  }
}

void LightSim::_observe_agents() {
  for (auto& agent : _agents) {
    agent.observe(_agents);
  }
}

void LightSim::_setup_agents() {
  std::uniform_real_distribution<double> d_x(0.0, _env->size_x - 1);
  std::uniform_real_distribution<double> d_y(0.0, _env->size_y - 1);
  std::uniform_int_distribution<uint32_t> d_ori(0, 359);

  for (auto& agent : _agents) {
    agent.coord = {d_x(_rd_gen), d_y(_rd_gen)};
    agent.orientation = d_ori(_rd_gen);
  }
}

uint32_t LightSim::eval_pred() {
  uint32_t temp = 0;

  for (auto const nb_prey : _preys_alive) {
    temp += _settings["preys"] - nb_prey;
  }

  _fitness_predator = temp;
  return _fitness_predator;
}

uint32_t LightSim::eval_prey() {
  uint32_t temp = 0;

  for (auto const nb_prey : _preys_alive) {
    temp += nb_prey;
  }

  _fitness_prey = temp;
  return _fitness_prey;
}

void LightSim::_reset_sim() {
  auto& set = _settings;

  _agents.clear();
  _preys_alive.clear();
  _preys_alive.resize(_settings["ticks"]);

  for (uint32_t i = 0; i < set["predators"]; ++i) {
    _agents.emplace_back(Predator(set["pred_speed"], set["pred_turn_speed"],
                                  set["pred_retina_cells"], set["pred_los"],
                                  set["pred_fov"]));
  }
  for (uint32_t i = 0; i < set["preys"]; ++i) {
    _agents.emplace_back(Prey(set["prey_speed"], set["prey_turn_speed"],
                              set["prey_retina_cells_by_layer"],
                              set["prey_los"], set["prey_fov"]));
  }
  _setup_agents();
}

void LightSim::_setup_sim() {
  auto& set = _settings;
  double w_scale = static_cast<double>(set["win_w"]) / set["grid_w"];
  double h_scale = static_cast<double>(set["win_h"]) / set["grid_h"];

  _env.reset(new Environment(set["grid_w"], set["grid_h"]));

  _reset_sim();

  _view.reset(set["headless"] == 0 ? (new MainView(set["win_w"], set["win_h"],
                                                   w_scale, h_scale, _agents))
                                   : nullptr);

  prey_mb = MarkovBrain2(set["prey_retina_cells_by_layer"] * 2, 2, true);
  pred_mb = MarkovBrain2(set["pred_retina_cells"], 2, true);
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

bool LightSim::_run() {
  return (_view != nullptr ? _run_ui() : _run_headless());
}

void LightSim::_sim_loop() {
  _capture_preys();
  _move_agents();
  _observe_agents();
  //_print_agents();

  _preys_alive[_tick] = (_agents.size() - _settings["predators"]);
}

bool LightSim::_run_ui() {
  using namespace std::chrono;
  steady_clock::time_point start, end;
  milliseconds delta;

  for (_tick = 0; _tick < _settings["ticks"]; ++_tick) {
    start = steady_clock::now();

    _sim_loop();

    _view->render();

    end = steady_clock::now();

    delta = milliseconds(17) - duration_cast<milliseconds>(end - start);
    if (delta > milliseconds(0)) {
      std::this_thread::sleep_for(delta);
    }
  }
  return true;
}

bool LightSim::_run_headless() {
  for (_tick = 0; _tick < _settings["ticks"]; ++_tick) {
    _sim_loop();
  }
  return true;
}
