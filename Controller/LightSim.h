#ifndef LIGHTSIM_H
#define LIGHTSIM_H

#include <Misc/Globals.h>

#include <Model/Agents/Predator.h>
#include <Model/Agents/Prey.h>
#include <Model/Environment/Environment.h>
#include <Model/Evo/MarkovBrain2.h>
#include <View/MainView.h>

namespace sim {
class LightSim {
 public:
  LightSim();
  ~LightSim();

  bool run();

  uint64_t init_pred_mn(uint32_t nb_actions, uint32_t nb_sensors);
  uint64_t init_prey_mn(uint32_t nb_actions, uint32_t nb_sensors);

  uint32_t eval_pred();
  uint32_t eval_prey();

  void save_pred_mn(std::string file_to_save_mn);
  void save_prey_mn(std::string file_to_save_mn);
  void save_pred_mn_from_seed(uint64_t seed, std::string file_to_save_mn);
  void save_prey_mn_from_seed(uint64_t seed, std::string file_to_save_mn);

  void evolve_pred(std::string file_from_load_mn, float alpha);
  void evolve_prey(std::string file_from_load_mn, float alpha);

  friend std::ostream& operator<<(std::ostream& os, LightSim const& lightsim);
  friend std::istream& operator>>(std::istream& os, LightSim& lightsim);

 private:
  std::map<std::string, uint32_t> _settings{{"headless", 0},
                                            {"threads", 1},
                                            {"win_w", 768},
                                            {"win_h", 768},
                                            {"ticks", 2000},
                                            {"grid_w", 512},
                                            {"grid_h", 512},
                                            {"predators", 1},
                                            {"preys", 50},
                                            {"pred_speed", 1},
                                            {"pred_turn_speed", 8},
                                            {"pred_los", 100},
                                            {"pred_fov", 180},
                                            {"pred_retina_cells", 12},
                                            {"prey_speed", 1},
                                            {"prey_turn_speed", 8},
                                            {"prey_los", 100},
                                            {"prey_fov", 180},
                                            {"prey_retina_cells_by_layer", 12},
                                            {"pred_generations", 10},
                                            {"pred_children", 10},
                                            {"prey_generations", 400},
                                            {"prey_children", 400},
                                            {"evolve_prey", 0},
                                            {"evolve_pred", 1}};
  MarkovBrain2 prey_mn;
  MarkovBrain2 pred_mn;

  Agents _agents;

  uint32_t _fitness_predator;
  uint32_t _fitness_prey;
  uint32_t _tick;

  uint64_t _seed;

  std::unique_ptr<MainView> _view;
  std::unique_ptr<Environment> _env;

  std::vector<uint32_t> _preys_alive;
  std::mt19937 _rd_gen;

  void _setup_sim();
  void _setup_agents();
  void _print_agents();
  void _move_agents();
  void _capture_preys();
  void _observe_agents();

  bool _run_ui();
  bool _run_headless();
  void _sim_loop();
};
}
#endif  // LIGHTSIM_H
