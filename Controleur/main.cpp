#include <Controleur/LightSim.h>
#include <Modele/Evo/Mn.h>

int main() {
  sim::LightSim lightSim(768, 768, 512, 512, 10, 200);
  lightSim.run(1e5);

  return EXIT_SUCCESS;
}
