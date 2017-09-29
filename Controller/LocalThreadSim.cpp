#include "LocalThreadSim.h"

using namespace sim;

LocalThreadSim::LocalThreadSim(std::map<std::string, uint32_t>& settings,
                               MarkovBrain& pred_mb,
                               MarkovBrain& prey_mb)
    : pred_mb(pred_mb), prey_mb(prey_mb), _settings(settings), _view(nullptr) {
  auto& set = _settings;
  double w_scale = static_cast<double>(set["win_w"]) / set["grid_w"];
  double h_scale = static_cast<double>(set["win_h"]) / set["grid_h"];

  _env.reset(new Environment(set["grid_w"], set["grid_h"]));

  if (set["headless"] == 0 && set["threads"] == 1) {
    _view = std::make_unique<MainView>(set["win_w"], set["win_h"], w_scale,
                                       h_scale, _agents);
  }
}

void LocalThreadSim::_setup_agents() {
  std::uniform_real_distribution<double> d_x(0.0, _env->size_x - 1);
  std::uniform_real_distribution<double> d_y(0.0, _env->size_y - 1);
  std::uniform_int_distribution<uint32_t> d_ori(0, 359);

  for (auto& agent : _agents) {
    agent->coord = {d_x(_rd_gen), d_y(_rd_gen)};
    agent->orientation = static_cast<int32_t>(d_ori(_rd_gen));
  }
}

void LocalThreadSim::_print_agents() {
  for (auto const& agent : _agents) {
    std::cout << *agent << std::endl;
  }
}

void LocalThreadSim::_move_agents() {
  std::vector<uint8_t> output;

  for (auto& agent : _agents) {
    agent->predates() ? output = pred_mb.actions(agent->get_state())
                      : output = prey_mb.actions(agent->get_state());

    if (output[0] != 0) {
      agent->turn_left();
    } else {
      agent->turned_left = false;
    }
    if (output[1] != 0) {
      agent->turn_right();
    } else {
      agent->turned_right = false;
    }

    auto temp_x = agent->coord.x + agent->speed * cos(agent->orientation);
    auto temp_y = agent->coord.y + agent->speed * sin(agent->orientation);

    if (_settings["torus"] == 1) {
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
    } else {
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
    }
    agent->coord.x = temp_x;
    agent->coord.y = temp_y;
  }
}

void LocalThreadSim::_capture_preys() {
  auto it_agent = _agents.begin();

  while (it_agent != _agents.end()) {
    auto& agent = *it_agent;
    if (agent->predates()) {
      if (agent->handling_time == 0) {
        _agents.erase(std::remove_if(
                          _agents.begin(), _agents.end(),
                          [&agent](auto& x) {
                            bool prey = !x->predates();
                            bool cap = false;

                            if (prey) {
                              cap = static_cast<Predator&>(*agent).captures(*x);
                            }
                            return cap;
                          }),
                      _agents.end());
      } else if (agent->handling_time > 0) {
        agent->handling_time--;
      }
    }

    ++it_agent;
  }
}

void LocalThreadSim::_observe_agents() {
  for (auto& agent : _agents) {
    agent->observe(_agents);
  }
}

uint32_t LocalThreadSim::eval_pred() {
  uint32_t fitness_predator = 0;
  uint32_t preys = _settings["preys"];

  for (auto const nb_prey : _preys_alive) {
    fitness_predator += preys - nb_prey;
  }

  return fitness_predator;
}

uint32_t LocalThreadSim::eval_prey() {
  uint32_t fitness_prey = 0;

  for (auto const nb_prey : _preys_alive) {
    fitness_prey += nb_prey;
  }

  return fitness_prey;
}

void LocalThreadSim::_reset_sim() {
  auto& set = _settings;
  std::random_device rd;

  _rd_gen.seed(rd());

  _agents.clear();
  _preys_alive.clear();
  _preys_alive.resize(set["ticks"]);

  for (uint32_t i = 0; i < set["predators"]; ++i) {
    _agents.emplace_back(std::make_unique<Predator>(
        set["pred_speed"], set["pred_turn_speed"], set["pred_retina_cells"],
        set["pred_los"], set["pred_fov"], set["predator_confusion"] == 1));
  }
  for (uint32_t i = 0; i < set["preys"]; ++i) {
    _agents.emplace_back(std::make_unique<Prey>(
        set["prey_speed"], set["prey_turn_speed"],
        set["prey_retina_cells_by_layer"], set["prey_los"], set["prey_fov"]));
  }
  _setup_agents();
}

void LocalThreadSim::_sim_loop(uint32_t tick) {
  _capture_preys();
  _move_agents();
  _observe_agents();
  //_print_agents();

  _preys_alive[tick] = _agents.size() - _settings["predators"];
}

bool LocalThreadSim::run() {
  _reset_sim();

  return (_view != nullptr ? _run_ui() : _run_headless());
}

bool LocalThreadSim::_run_ui() {
  using namespace std::chrono;
  steady_clock::time_point start, end;
  milliseconds delta;
  uint32_t tick = 0;
  uint32_t ticks = _settings["ticks"];

  for (tick = 0; tick < ticks; ++tick) {
    start = steady_clock::now();

    _sim_loop(tick);

    _view->render();

    end = steady_clock::now();

    delta = milliseconds(17) - duration_cast<milliseconds>(end - start);
    if (delta > milliseconds(0)) {
      std::this_thread::sleep_for(delta);
    }
  }
  return true;
}

bool LocalThreadSim::_run_headless() {
  uint32_t tick = 0;
  uint32_t ticks = _settings["ticks"];

  for (tick = 0; tick < ticks; ++tick) {
    _sim_loop(tick);
  }
  return true;
}
