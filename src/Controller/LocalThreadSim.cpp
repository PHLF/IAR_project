#include "LocalThreadSim.h"

#include <algorithm>
#include <chrono>
#include <thread>

#include "Model/Agents/Predator.h"
#include "Model/Agents/Prey.h"
#include "Model/Environment/box.h"
#include "Model/Environment/torus.h"

using namespace sim;

LocalThreadSim::LocalThreadSim(const toml::table& settings,
                               MarkovBrain& pred_mb,
                               MarkovBrain& prey_mb)
    : pred_mb(pred_mb), prey_mb(prey_mb), _settings(settings), _view(nullptr) {
  const auto viewport = _settings["viewport"];
  const auto simulation = _settings["simulation"];

  const auto universe_width =
      simulation["universe"]["width"].as_integer()->get();
  const auto universe_height =
      simulation["universe"]["height"].as_integer()->get();

  if (simulation["universe"]["closed curvature"].as_boolean()->get()) {
    _env.reset(new Torus(universe_width, universe_height));

  } else {
    _env.reset(new Box(universe_width, universe_height));
  }

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
                                       w_scale, h_scale, _agents);
  }

  _ticks_per_run = simulation["ticks"].as_integer()->get();
  _nb_preys = _settings["prey"]["number"].as_integer()->get();
  _nb_predators = _settings["predator"]["number"].as_integer()->get();
}

void LocalThreadSim::_setup_agents() {
  std::uniform_int_distribution<int32_t> d_x(
      0, static_cast<int32_t>(_env->size_x) - 1);
  std::uniform_int_distribution<int32_t> d_y(
      0, static_cast<int32_t>(_env->size_y) - 1);
  std::uniform_int_distribution<uint32_t> d_ori(0, 359);

  for (auto& agent : _agents) {
    agent->coord = {d_x(_rd_gen), d_y(_rd_gen)};
    agent->orientation = d_ori(_rd_gen);
  }
}

void LocalThreadSim::_print_agents() {
  for (auto const& agent : _agents) {
    std::cout << *agent << std::endl;
  }
}

uint32_t LocalThreadSim::eval_pred() {
  uint32_t fitness_predator = 0;
  const auto preys = _nb_preys;

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
  _preys_alive.resize(_ticks_per_run);

  const auto& predator = _settings["predator"];

  const auto pred_speed = predator["speed"].as_integer()->get();
  const auto pred_turn_rate = predator["turn rate"].as_integer()->get();
  const auto pred_retina_cells =
      predator["sight"]["retina cells"].as_integer()->get();
  const auto pred_los = predator["sight"]["line of sight"].as_integer()->get();
  const auto pred_fov = predator["sight"]["field of view"].as_integer()->get();
  const auto pred_confusion = predator["confusion"].as_boolean()->get();

  for (uint32_t i = 0; i < _nb_predators; ++i) {
    _agents.emplace_back(std::make_unique<Predator>(
        pred_mb, pred_speed, pred_turn_rate, pred_retina_cells, pred_los,
        pred_fov, pred_confusion,
        _view != nullptr ? _view->pred_sprite().get() : nullptr));
  }

  const auto& prey = _settings["prey"];

  const auto prey_speed = prey["speed"].as_integer()->get();
  const auto prey_turn_rate = prey["turn rate"].as_integer()->get();
  const auto prey_retina_cells =
      prey["sight"]["retina cells by agent type"].as_integer()->get();
  const auto prey_los = prey["sight"]["line of sight"].as_integer()->get();
  const auto prey_fov = prey["sight"]["field of view"].as_integer()->get();

  for (uint32_t i = 0; i < _nb_preys; ++i) {
    _agents.emplace_back(std::make_unique<Prey>(
        prey_mb, prey_speed, prey_turn_rate, prey_retina_cells, prey_los,
        prey_fov, _view != nullptr ? _view->prey_sprite().get() : nullptr));
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
    ++it_agent;
  }
  //_print_agents();

  _preys_alive[tick] = _agents.size() - _nb_predators;
}

bool LocalThreadSim::run() {
  _reset_sim();

  return (_view != nullptr ? _run_ui() : _run_headless());
}

bool LocalThreadSim::_run_ui() {
  using namespace std::chrono;
  using namespace std::chrono_literals;

  steady_clock::time_point start, end;

  uint32_t tick = 0;
  const uint32_t ticks = _ticks_per_run;

  for (tick = 0; tick < ticks; ++tick) {
    start = steady_clock::now();

    _sim_loop(tick);

    _view->render();

    end = steady_clock::now();

    std::this_thread::sleep_for(16.67ms - (end - start));
  }
  return true;
}

bool LocalThreadSim::_run_headless() {
  uint32_t tick = 0;
  const uint32_t ticks = _ticks_per_run;

  for (tick = 0; tick < ticks; ++tick) {
    _sim_loop(tick);
  }
  return true;
}
