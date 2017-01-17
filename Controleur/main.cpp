#include <Controleur/LightSim.h>

int main() {
  sim::LightSim lightSim(1280, 1024, 512, 512, 1, 50);
  lightSim.run(1e5);

  return EXIT_SUCCESS;
}
