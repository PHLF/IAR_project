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

bool LightSim::run(uint32_t nbTicks) {
  for (_tick = 0; _tick < nbTicks; ++_tick) {
    std::cout << "Tick n°" << _tick << std::endl;
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
