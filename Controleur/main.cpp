#include <Controleur/LightSim.h>
#include <Modele/Evo/Mn.h>

int main() {
  //sim::LightSim lightSim(768, 768, 512, 512, 10, 200);
  //lightSim.run(1e5);
  sim::Mn mn(2,2);
  mn.print_tirages();
  std::cout << " " << std::endl;
  mn.gaussian_random_mutation(0.1);
  mn.print_tirages();


  return EXIT_SUCCESS;
}
