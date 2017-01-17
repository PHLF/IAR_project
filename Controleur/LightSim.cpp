#include <Controleur/LightSim.h>

using namespace sim;

//extern double sin(uint32_t angle_in_degrees);
//extern double cos(uint32_t angle_in_degrees);

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
  // TODO
  for (auto& agent : _env->get_agents()) {
    std::cout << *agent << std::endl;
  }
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
  // TODO
  for (auto& agent : _env->get_agents()) {
    agent->get_retina();
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
