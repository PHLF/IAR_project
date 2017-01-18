#include <Controleur/LightSim.h>

int main() {
  sim::LightSim lightSim(768,768,512, 512, 10, 100);
  lightSim.run(1e5);

  return EXIT_SUCCESS;
}
