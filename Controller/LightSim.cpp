#include <Controller/LightSim.h>

using namespace sim;

LightSim::LightSim(uint32_t win_w,
                   uint32_t win_h,
                   uint32_t grid_x,
                   uint32_t grid_y,
                   uint32_t nb_predators,
                   uint32_t nb_preys)
    : _env(new Environment(grid_x, grid_y, nb_predators, nb_preys)),
      _fen(new MainView(win_w,
                        win_h,
                        static_cast<double>(win_w) / grid_x,
                        static_cast<double>(win_h) / grid_y,
                        _env->get_agents())) {}

LightSim::LightSim(uint32_t grid_x,
                   uint32_t grid_y,
                   uint32_t nb_predators,
                   uint32_t nb_preys)
    : _env(new Environment(grid_x, grid_y, nb_predators, nb_preys)),
      _fen(nullptr) {}

LightSim::~LightSim() {}

double LightSim::_random_x() {
  std::uniform_real_distribution<double> distrib_x(0.0, _env->size_x - 1);

  return distrib_x(_generator);
}

double LightSim::_random_y() {
  std::uniform_real_distribution<double> distrib_y(0.0, _env->size_y - 1);

  return distrib_y(_generator);
}

uint32_t LightSim::_random_orientation() {
  std::uniform_int_distribution<uint32_t> distrib_ori(0, 359);

  return distrib_ori(_generator);
}

void LightSim::_print_agents() {
  for (auto& agent : _env->get_agents()) {
    std::cout << *agent << std::endl;
  }
}

bool LightSim::_areClockwise(Coords v1, Coords v2) {
  return -v1.x * v2.y + v1.y * v2.x > 0;
}

bool LightSim::_isWithinRadius(Coords v, uint32_t radiusSquared) {
  return v.x * v.x + v.y * v.y <= radiusSquared;
}

bool LightSim::_isInsideSector(Coords point,
                               Coords center,
                               Coords sectorStart,
                               Coords sectorEnd,
                               uint32_t radiusSquared) {
  Coords relPoint{point.x - center.x, point.y - center.y};

  return !_areClockwise(sectorStart, relPoint) &&
         _areClockwise(sectorEnd, relPoint) &&
         _isWithinRadius(relPoint, radiusSquared);
}

void LightSim::_move_agents() {
  std::vector<uint8_t> output;

  for (auto& agent : _env->get_agents()) {
    agent->predates ? output = pred_mn.choose_action(agent->get_input())
                    : output = prey_mn.choose_action(agent->get_input());

    if (output[0] != 0) {
      agent->turnLeft();
    } else {
      agent->turned_left = false;
    }
    if (output[1] != 0) {
      agent->turnRight();
    } else {
      agent->turned_right = false;
    }

    auto temp_x = agent->coord.x + agent->speed * cos(agent->orientation);
    auto temp_y = agent->coord.y + agent->speed * sin(agent->orientation);

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
    agent->coord.x = temp_x;
    agent->coord.y = temp_y;
  }
}

void LightSim::_capture_preys() {
  auto& agents = _env->get_agents();
  auto it_agent = agents.begin();

  while (it_agent != agents.end()) {
    if ((*it_agent)->predates && (*it_agent)->handling_time == 0) {
      agents.erase(
          std::remove_if(agents.begin(), agents.end(),
                         [&it_agent, this](std::unique_ptr<Agent>& x) {
                           uint32_t num_prey = 0;
                           bool prey = !x->predates;
                           bool same_coord = false;
                           bool cap = false;
                           std::uniform_int_distribution<uint8_t> d_cap(0, 100);

                           if (prey) {
                             same_coord =
                                 is_near((*it_agent)->coord, x->coord, 5);
                           }
                           if (prey && same_coord) {
                             for (auto const visual_stimuli :
                                  (*it_agent)->get_retina()->cells_preys) {
                               if (visual_stimuli != 0) {
                                 ++num_prey;
                               }
                             }
                             auto tirage = d_cap(_generator);
                             if (num_prey > 0 && (tirage <= (100 / num_prey))) {
                               _nb_captures++;
                               cap = true;
                               (*it_agent)->handling_time = 10;
                             }
                           }
                           return cap;
                         }),
          agents.end());
    } else if ((*it_agent)->predates && (*it_agent)->handling_time > 0) {
      (*it_agent)->handling_time--;
    }

    ++it_agent;
  }
}

void LightSim::_observe_agents() {
  Coords sector_start;
  Coords sector_end;
  std::vector<Agent*> temp_agents;

  for (const auto& agent_i : _env->get_agents()) {
    const auto& retina = agent_i->get_retina();
    retina->clear();
    retina->compute_local_vectors(agent_i->coord, agent_i->orientation);

    sector_start = retina->get_view_vectors().front();
    sector_end = retina->get_view_vectors().back();

    for (const auto& agent_j : _env->get_agents()) {
      if (agent_i != agent_j) {
        if (_isInsideSector(agent_j->coord, agent_i->coord, sector_start,
                            sector_end,
                            retina->getDepth() * retina->getDepth())) {
          temp_agents.push_back(agent_j.get());
        }
      }
    }

    for (const auto& agent_j : temp_agents) {
      for (uint32_t i = 0; i < retina->get_nb_segments(); ++i) {
        sector_start = retina->get_view_vectors()[i];
        sector_end = retina->get_view_vectors()[i + 1];

        if (_isInsideSector(agent_j->coord, agent_i->coord, sector_start,
                            sector_end,
                            retina->getDepth() * retina->getDepth())) {
          agent_j->predates ? retina->cells_predators[i] = 1
                            : retina->cells_preys[i] = 1;
        }
      }
    }
    temp_agents.clear();
  }
}

void LightSim::_setup_agents() {
  for (auto& agent : _env->get_agents()) {
    agent->coord = {_random_x(), _random_y()};
    agent->orientation = _random_orientation();
  }
}

uint32_t LightSim::eval_pred() {
  uint32_t temp = 0;

  for (auto const nb_prey : _preys_alive) {
    temp += _env->get_nb_preys() - nb_prey;
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

void LightSim::evolve_pred(std::string file_from_load_mn, float alpha) {
  pred_mn.load_file(file_from_load_mn);
  pred_mn.gaussian_random_mutation(alpha);
}

void LightSim::evolve_prey(std::string file_from_load_mn, float alpha) {
  prey_mn.load_file(file_from_load_mn);
  prey_mn.gaussian_random_mutation(alpha);
}

bool LightSim::run(uint32_t nbTicks) {
  _preys_alive.resize(nbTicks);
  _env->reset();
  _setup_agents();
  //  _print_agents();
  return (_fen != nullptr ? run_ui(nbTicks) : run_headless(nbTicks));
}

uint64_t LightSim::init_pred_mn(uint32_t nb_actions, uint32_t nb_sensors) {
  uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();

  pred_mn.set_dim(nb_actions, nb_sensors);
  pred_mn.random_fill(seed);

  return seed;
}

uint64_t LightSim::init_prey_mn(uint32_t nb_actions, uint32_t nb_sensors) {
  uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();

  prey_mn.set_dim(nb_actions, nb_sensors);
  prey_mn.random_fill(seed);

  return seed;
}

void LightSim::save_pred_mn_from_seed(uint64_t seed,
                                      std::string file_to_save_mn) {
  pred_mn.random_fill(seed);
  pred_mn.save_as_file(file_to_save_mn);
}

void LightSim::save_prey_mn_from_seed(uint64_t seed,
                                      std::string file_to_save_mn) {
  prey_mn.random_fill(seed);
  pred_mn.save_as_file(file_to_save_mn);
}

void LightSim::save_pred_mn(std::string file_to_save_mn) {
  pred_mn.save_as_file(file_to_save_mn);
}

void LightSim::save_prey_mn(std::string file_to_save_mn) {
  pred_mn.save_as_file(file_to_save_mn);
}

bool LightSim::run_headless(uint32_t nbTicks) {
  using namespace std::chrono;
  _generator.seed(system_clock::now().time_since_epoch().count());

  for (_tick = 0; _tick < nbTicks; ++_tick) {
    _capture_preys();
    _move_agents();
    _observe_agents();

    _preys_alive[_tick] =
        (_env->get_agents().size() - _env->get_nb_predators());

    //    _print_agents();
  }
  return true;
}

bool LightSim::run_ui(uint32_t nbTicks) {
  using namespace std::chrono;
  steady_clock::time_point start, end;
  milliseconds delta;

  _generator.seed(system_clock::now().time_since_epoch().count());

  for (_tick = 0; _tick < nbTicks; ++_tick) {
    start = steady_clock::now();

    _capture_preys();
    _move_agents();
    _observe_agents();
    //_print_agents();
    _fen->render();

    _preys_alive[_tick] =
        (_env->get_agents().size() - _env->get_nb_predators());

    end = steady_clock::now();

    delta = milliseconds(17) - duration_cast<milliseconds>(end - start);
    if (delta > milliseconds(0)) {
      std::this_thread::sleep_for(delta);
    }
  }
  return true;
}

std::ostream& operator<<(std::ostream& strm, const LightSim& a) {
  // return strm << "A(" << a.i << ")" << std::endl;
  return strm;
}
