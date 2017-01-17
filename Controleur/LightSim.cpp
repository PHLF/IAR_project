#include <Controleur/LightSim.h>

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
                                 win_w / grid_x,
                                 win_h / grid_y,
                                 _env->get_agents())) {
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
  for (auto& agent : _env->get_agents()) {
    std::cout << *agent << std::endl;
  }
}

void LightSim::_move_agents() {
  // TODO
  for (auto& agent : _env->get_agents()) {
    agent->moveForward();
  }
}

void LightSim::_observe_agents() {
  // TODO
  for (auto& agent : _env->get_agents()) {
    agent->observe();
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

  _generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

  _setup_agents();
  _print_agents();

  for (_tick = 0; _tick < nbTicks; ++_tick) {
    std::cout << "Tick n°" << _tick << std::endl;

    auto start = steady_clock::now();
    _observe_agents();
    _move_agents();
    //_print_agents();

    _fen->render();
    auto end = steady_clock::now();
    auto delta = duration_cast<milliseconds>(end - start);

    if (milliseconds(16) - delta > 0) {
      std::this_thread::sleep_for(milliseconds(16) - delta);
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
