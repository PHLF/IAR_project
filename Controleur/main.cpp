#include <Controleur/LightSim.h>
#include <Modele/Evo/Mn.h>

int main() {
  sim::LightSim lightSim(768, 768, 512, 512, 10, 10);
  lightSim.run(3e2);

  return EXIT_SUCCESS;
}
