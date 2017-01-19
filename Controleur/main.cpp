#include <Controleur/LightSim.h>
#include <Modele/Evo/Mn.h>

int main() {
  //  To enable display
  //    sim::LightSim lightSim(768,768,512, 512, 1, 50);
  sim::LightSim lightSim(512, 512, 1, 50);
  std::map<uint32_t, std::string> fitness_and_mn;
  std::stringstream filename;
  std::string best_predator;

  for (uint32_t generation = 0; generation < 1000; ++generation) {
    std::cout << "Generation: " << generation << std::endl;
    for (uint32_t child = 0; child < 10; ++child) {
      filename << "Predators/pred_" << generation << "_" << child << ".bin";
      lightSim.run(2000);

      fitness_and_mn.emplace(lightSim.eval_pred(filename.str()),
                             filename.str());

      filename.str("");
      filename.clear();
    }
    best_predator = fitness_and_mn[fitness_and_mn.rbegin()->first];

    for (auto const& fitness_and_file : fitness_and_mn) {
      if (fitness_and_file.second != best_predator) {
        std::remove(fitness_and_file.second.c_str());
      }
    }
    fitness_and_mn.clear();
    lightSim.evolve_pred(best_predator, 0.5);
  }

  return EXIT_SUCCESS;
}
