﻿#include <Controleur/LightSim.h>

using namespace sim;

LightSim::LightSim(uint32_t win_w,
                   uint32_t win_h,
                   uint32_t grid_x,
                   uint32_t grid_y,
                   uint32_t nb_predators,
                   uint32_t nb_preys)
    : _env(new Environment(grid_x, grid_y, nb_predators, nb_preys)),
      _fen(new FenetrePrincipale(win_w,
                                 win_h,
                                 static_cast<double>(win_w) / grid_x,
                                 static_cast<double>(win_h) / grid_y,
                                 _env->get_agents())) {
  // grid = new Agent* [sizex];
}

LightSim::~LightSim() {
  /*for (uint32_t i=0; i < sizex; i++){
      grid[i] = new Agent[ sizey ];
  }*/
}

float LightSim::_random_x() {
  std::uniform_real_distribution<float> distrib_x(0.0, _env->size_x - 1);

  return distrib_x(_generator);
}

float LightSim::_random_y() {
  std::uniform_real_distribution<float> distrib_y(0.0, _env->size_y - 1);

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
  for (auto& agent : _env->get_agents()) {
    auto temp_x = agent->get_coord().x +
                  agent->get_speed() * cos(agent->get_orientation());
    auto temp_y = agent->get_coord().y +
                  agent->get_speed() * sin(agent->get_orientation());

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
    agent->get_coord().x = temp_x;
    agent->get_coord().y = temp_y;
  }
}

void LightSim::_observe_agents() {
  Coords sector_start;
  Coords sector_end;
  std::vector<Agent*> temp_agents;

  for (const auto& agent_i : _env->get_agents()) {
    const auto& retina = agent_i->get_retina();
    retina->clear();
    retina->compute_local_vectors(agent_i->get_coord(),
                                  agent_i->get_orientation());

    sector_start = retina->get_view_vectors().front();
    sector_end = retina->get_view_vectors().back();

    for (const auto& agent_j : _env->get_agents()) {
      if (agent_i == agent_j) {
        break;
      }

      if (_isInsideSector(agent_j->get_coord(), agent_i->get_coord(),
                          sector_start, sector_end,
                          retina->getDepth() * retina->getDepth())) {
        temp_agents.push_back(agent_j.get());
      }
    }

    for (const auto& agent_j : temp_agents) {
      for (uint32_t i = 0; i < retina->get_nb_segments(); ++i) {
        sector_start = retina->get_view_vectors()[i];
        sector_end = retina->get_view_vectors()[i + 1];

        if (_isInsideSector(agent_j->get_coord(), agent_i->get_coord(),
                            sector_start, sector_end,
                            retina->getDepth() * retina->getDepth())) {
          agent_j->predates ? retina->cells_predators[i] = true
                            : retina->cells_preys[i] = true;
        }
      }
    }
    temp_agents.clear();
  }
}

void LightSim::_setup_agents() {
  // TODO
  for (auto& agent : _env->get_agents()) {
    agent->set_coord({_random_x(), _random_y()});
    agent->set_orientation(_random_orientation());
  }
}

bool LightSim::run(uint32_t nbTicks) {
  using namespace std::chrono;
  steady_clock::time_point start, end;
  milliseconds delta;

  _generator.seed(system_clock::now().time_since_epoch().count());

  _setup_agents();
  _print_agents();

  for (_tick = 0; _tick < nbTicks; ++_tick) {
    std::cout << "Tick n°" << _tick << std::endl;

    start = steady_clock::now();
    _observe_agents();
    _move_agents();
    //_print_agents();
    _fen->render();
    end = steady_clock::now();

    delta = milliseconds(17) - duration_cast<milliseconds>(end - start);
    if (delta > milliseconds(0)) {
      std::this_thread::sleep_for(delta);
    }
  }

  return true;
}

/**
 * @brief operator << Affichage de l'environnement sous forme de grille pour
 * débug
 * @param strm
 * @param a
 * @return
 */
std::ostream& operator<<(std::ostream& strm, const LightSim& a) {
  // return strm << "A(" << a.i << ")" << std::endl;
  return strm;
}
