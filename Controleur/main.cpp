#include <Controleur/LightSim.h>

int main() {
  sim::LightSim lightSim(768, 768, 512, 512, 1, 50);
  lightSim.run(1e3);

  return EXIT_SUCCESS;
}
