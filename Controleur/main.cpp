#include <Controleur/LightSim.h>
#include <Modele/Evo/Mn.h>

int main() {
  sim::LightSim lightSim(512, 512, 1, 50);
  std::map<uint32_t, std::string> fitness_and_mn;
  std::stringstream filename;

  for (uint32_t generation = 0; generation < 1000; ++generation) {
    for (uint32_t child = 0; child < 100; ++child) {
      filename << "Predators/pred_" << child << ".bin";
      lightSim.run(2000);
      fitness_and_mn.emplace(lightSim.eval_pred(filename.str()),
                             filename.str());

      filename.str("");
      filename.clear();
    }
    auto best_predator = fitness_and_mn[fitness_and_mn.rbegin()->first];

    for (auto const& fitness_and_file : fitness_and_mn) {
      if (fitness_and_file.second != best_predator) {
        std::remove(fitness_and_file.second.c_str());
      }
    }
  }

  return EXIT_SUCCESS;
}
