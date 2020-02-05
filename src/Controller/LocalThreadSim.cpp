#include "LocalThreadSim.h"

#include <algorithm>
#include <chrono>
#include <thread>

#include "Model/Environment/box.h"
#include "Model/Environment/torus.h"
#include "Model/Agents/Predator.h"
#include "Model/Agents/Prey.h"

using namespace sim;

LocalThreadSim::LocalThreadSim(std::map<std::string, uint32_t>& settings,
                               MarkovBrain& pred_mb,
                               MarkovBrain& prey_mb)
    : pred_mb(pred_mb), prey_mb(prey_mb), _settings(settings), _view(nullptr) {
  auto& set = _settings;
  double w_scale = static_cast<double>(set["win_w"]) / set["grid_w"];
  double h_scale = static_cast<double>(set["win_h"]) / set["grid_h"];

  if (_settings["torus"] == 1) {
    _env.reset(new Torus(set["grid_w"], set["grid_h"]));

  } else {
    _env.reset(new Box(set["grid_w"], set["grid_h"]));
  }

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
        pred_mb, set["pred_speed"], set["pred_turn_speed"],
        set["pred_retina_cells"], set["pred_los"], set["pred_fov"],
        set["predator_confusion"] == 1,
        _view != nullptr ? _view->pred_sprite().get() : nullptr));
  }
  for (uint32_t i = 0; i < set["preys"]; ++i) {
    _agents.emplace_back(std::make_unique<Prey>(
        prey_mb, set["prey_speed"], set["prey_turn_speed"],
        set["prey_retina_cells_by_layer"], set["prey_los"], set["prey_fov"],
        _view != nullptr ? _view->prey_sprite().get() : nullptr));
  }
  _setup_agents();
}

void LocalThreadSim::_sim_loop(uint32_t tick) {
  for (auto& agent : _agents) {
    agent->move(*_env);
    agent->observe(_agents);
  }

  auto it_agent = std::begin(_agents);
  while (it_agent != _agents.end()) {
    auto& agent = *it_agent;
    _agents.erase(
        std::remove_if(_agents.begin(), _agents.end(),
                       [&agent](auto& x) { return agent->captures(*x); }),
        std::end(_agents));
  }
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

    std::this_thread::sleep_for(delta);
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
