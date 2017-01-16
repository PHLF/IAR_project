#include <Controleur/LightSim.h>

LightSim::LightSim(uint32_t x,
                   uint32_t y,
                   uint32_t nbPredators,
                   uint32_t nbPreys)
    : _env(new Environment(x, y, nbPredators, nbPreys)) {
  // grid = new Agent* [sizex];
}

LightSim::~LightSim() {
  /*for (uint32_t i=0; i < sizex; i++){
      grid[i] = new Agent[ sizey ];
  }*/
}

float LightSim::_random_x() {
  std::uniform_real_distribution<float> distrib_x(0.0, _env->get_size_x() - 1);

  return distrib_x(_generator);
}

float LightSim::_random_y() {
  std::uniform_real_distribution<float> distrib_y(0.0, _env->get_size_y() - 1);

  return distrib_y(_generator);
}

uint32_t LightSim::_random_orientation() {
  std::uniform_int_distribution<uint32_t> distrib_ori(0, 359);

  return distrib_ori(_generator);
}

void LightSim::_print_agents() {
  // TODO
  for (auto& agent : _env->get_predators()) {
    std::cout << *agent << std::endl;
  }
  for (auto& agent : _env->get_preys()) {
    std::cout << *agent << std::endl;
  }
}

void LightSim::_move_agents() {
  // TODO
  for (auto& agent : _env->get_predators()) {
    agent->moveForward();
  }
  //  for (auto& agent : _env->get_preys()) {
  //    agent->moveForward();
  //  }
}

void LightSim::_observe_agents() {
  // TODO
  for (auto& agent : _env->get_preys()) {
    agent->get_retina();
  }
  for (auto& agent : _env->get_predators()) {
    agent->get_retina();
  }
}

void LightSim::_setup_agents() {
  // TODO
  for (auto& agent : _env->get_preys()) {
    agent->set_coord({_random_x(), _random_y()});
    agent->set_orientation(_random_orientation());
  }
  for (auto& agent : _env->get_predators()) {
    agent->set_coord({_random_x(), _random_y()});
    agent->set_orientation(_random_orientation());
  }
}

bool LightSim::run(uint32_t nbTicks) {
  _generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

  //  Agents& preds = _env->get_predators();
  //  Agents& preys = _env->get_preys();

  _setup_agents();
  _print_agents();

  for (_tick = 0; _tick < nbTicks; ++_tick) {
    std::cout << "Tick n°" << _tick << std::endl;
    _move_agents();
    _print_agents();
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
