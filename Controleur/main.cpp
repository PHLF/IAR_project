#include <Controleur/LightSim.h>
#include <Modele/Evo/Mn.h>

int main() {
  sim::LightSim lightSim(768, 768, 512, 512, 1, 200);
  lightSim.run(2e3);

  return EXIT_SUCCESS;
}
