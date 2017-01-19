#include <Controleur/LightSim.h>
#include <Modele/Evo/Mn.h>

struct Settings {
  // App related settings
  bool headless = false;
  uint32_t threads = 1;
  uint32_t win_w = 512;
  uint32_t win_h = 512;

  // Simulation related settings
  uint32_t grid_w = 512;
  uint32_t grid_h = 512;
  uint32_t predators = 1;
  uint32_t predators_actions = 2;
  uint32_t predators_sensors = 12;
  uint32_t preys = 50;
  uint32_t prey_actions = 2;
  uint32_t prey_sensors = 24;

  // Evolution related settings
  bool evolve_pred = false;
  float sigma_pred = 0.5;
  uint32_t pred_generations = 100;
  uint32_t pred_children = 100;
  bool evolve_preys = false;
  float sigma_prey = 0.5;
  uint32_t prey_generations = 100;
  uint32_t prey_children = 100;
};

void load_parameters(Settings& param) {
  std::ifstream settings;
  std::stringstream iss;
  std::string line;

  settings.open("settings.txt", std::ios::in);

  if (settings.is_open()) {
    iss << settings.rdbuf();
    iss >> line >> std::boolalpha >> param.headless;
    iss >> line >> param.threads;
    iss >> line >> param.win_w;
    iss >> line >> param.win_h;
    iss >> line >> param.grid_w;
    iss >> line >> param.grid_h;
    iss >> line >> param.predators;
    iss >> line >> param.predators_actions;
    iss >> line >> param.predators_sensors;
    iss >> line >> param.preys;
    iss >> line >> param.prey_actions;
    iss >> line >> param.prey_sensors;
    iss >> line >> std::boolalpha >> param.evolve_pred;
    iss >> line >> param.sigma_pred;
    iss >> line >> param.pred_generations;
    iss >> line >> param.pred_children;
    iss >> line >> std::boolalpha >> param.evolve_preys;
    iss >> line >> param.sigma_prey;
    iss >> line >> param.prey_generations;
    iss >> line >> param.prey_children;

    settings.close();
  } else {
    std::cout << "Not settings.txt found, falling back to default values."
              << std::endl;
  }
  std::cout << "Loaded values: " << std::endl
            << "Headless: " << (param.headless ? "true" : "false") << std::endl
            << "Window' size (if !headless): (width: " << param.win_w
            << ", height: " << param.win_h << ")" << std::endl
            << "Thread(s): " << param.threads << std::endl
            << "Grid' size: (width: " << param.grid_w
            << ", height: " << param.grid_h << ")" << std::endl
            << "Number of preys: " << param.preys << std::endl
            << "Number of actions for preys: " << param.prey_actions
            << std::endl
            << "Number of sensors for preys: " << param.prey_sensors
            << std::endl
            << "Number of predators: " << param.predators << std::endl
            << "Number of actions for predators: " << param.predators_actions
            << std::endl
            << "Number of sensors for predators: " << param.predators_sensors
            << std::endl
            << "Evolve predators: " << (param.evolve_pred ? "true" : "false")
            << std::endl
            << "Sigma predators normal gene distribution: " << param.sigma_pred
            << std::endl
            << "Predators' generations: " << param.pred_generations << std::endl
            << "Predators' children: " << param.pred_children << std::endl
            << "Evolve preys: " << (param.evolve_preys ? "true" : "false")
            << std::endl
            << "Sigma preys normal gene distribution: " << param.sigma_pred
            << std::endl
            << "Preys' generations: " << param.prey_generations << std::endl
            << "Preys' children: " << param.prey_children << std::endl
            << std::endl;
}

int main() {
  Settings param;
  std::map<uint32_t, std::string> fitness_and_mn_file;
  std::map<uint32_t, uint64_t> fitness_and_seed;
  std::stringstream filename;
  std::string best_predator;
  std::string best_prey;
  uint64_t best_seed;
  uint64_t seed;

  std::ofstream pred_fit_val_file("pred_fitness_values.txt", std::ios::out);
  std::ofstream prey_fit_val_file("prey_fitness_values.txt", std::ios::out);

  load_parameters(param);

  std::unique_ptr<sim::LightSim> light_sim;

  if (param.headless) {
    light_sim.reset(new sim::LightSim(param.grid_w, param.grid_h,
                                      param.predators, param.preys));
  } else {
    light_sim.reset(new sim::LightSim(param.win_w, param.win_h, param.grid_w,
                                      param.grid_h, param.predators,
                                      param.preys));
  }

  if (param.evolve_pred) {
    light_sim->init_prey_mn(param.prey_actions,
                            (param.prey_sensors + param.predators_actions));
    for (uint32_t generation = 0; generation < param.pred_generations;
         ++generation) {
      std::cout << "Generation: " << generation << std::endl;

      if (generation == 0) {
        for (uint32_t child = 0; child < param.pred_children; ++child) {
          std::cout << "Predator: " << child << std::endl;
          seed = light_sim->init_pred_mn(
              param.predators_actions,
              (param.predators_actions + param.predators_sensors));
          light_sim->run(2000);
          fitness_and_seed.emplace(light_sim->eval_pred(), seed);
        }
        best_seed = fitness_and_seed.rbegin()->second;
        filename << "Predators/best_seed_" << best_seed << ".bin";

        best_predator = filename.str();
        light_sim->save_pred_mn_from_seed(best_seed, best_predator);

        // Save fitness value to file, for monitoring.
        pred_fit_val_file << fitness_and_seed.rbegin()->first << std::endl;

        filename.str("");
        filename.clear();
        fitness_and_seed.clear();
      } else {
        for (uint32_t child = 0; child < param.pred_children; ++child) {
          std::cout << "Predator: " << child << std::endl;
          filename << "Predators/pred_" << generation << "_" << child << ".bin";

          light_sim->evolve_pred(best_predator, param.sigma_pred);
          light_sim->run(2000);
          fitness_and_mn_file.emplace(light_sim->eval_pred(), filename.str());

          filename.str("");
          filename.clear();
        }
        best_predator = fitness_and_mn_file.rbegin()->second;
        light_sim->save_pred_mn(best_predator);

        // Save fitness value to file, for monitoring.
        pred_fit_val_file << fitness_and_mn_file.rbegin()->first << std::endl;

        fitness_and_mn_file.clear();
      }
    }
  }

  if (param.evolve_preys) {
    light_sim->init_pred_mn(param.predators_actions, (param.predators_sensors +
                                                      param.predators_actions));
    for (uint32_t generation = 0; generation < param.prey_generations;
         ++generation) {
      std::cout << "Generation: " << generation << std::endl;

      if (generation == 0) {
        for (uint32_t child = 0; child < param.prey_children; ++child) {
          std::cout << "Prey: " << child << std::endl;
          seed = light_sim->init_prey_mn(
              param.prey_actions, (param.prey_sensors + param.prey_actions));
          light_sim->run(2000);
          fitness_and_seed.emplace(light_sim->eval_prey(), seed);
        }
        best_seed = fitness_and_seed.rbegin()->second;
        filename << "Preys/best_seed_" << best_seed << ".bin";

        best_prey = filename.str();
        light_sim->save_prey_mn_from_seed(best_seed, best_prey);

        // Save fitness value to file, for monitoring.
        prey_fit_val_file << fitness_and_seed.rbegin()->first << std::endl;

        filename.str("");
        filename.clear();
        fitness_and_seed.clear();
      } else {
        for (uint32_t child = 0; child < param.prey_children; ++child) {
          std::cout << "Prey: " << child << std::endl;
          filename << "Preys/prey_" << generation << "_" << child << ".bin";

          light_sim->evolve_prey(best_prey, param.sigma_prey);
          light_sim->run(2000);
          fitness_and_mn_file.emplace(light_sim->eval_prey(), filename.str());

          filename.str("");
          filename.clear();
        }
        best_prey = fitness_and_mn_file.rbegin()->second;
        light_sim->save_prey_mn(best_prey);

        // Save fitness value to file, for monitoring.
        prey_fit_val_file << fitness_and_mn_file.rbegin()->first << std::endl;

        fitness_and_mn_file.clear();
      }
    }
  }

  pred_fit_val_file.close();
  prey_fit_val_file.close();

  return EXIT_SUCCESS;
}
