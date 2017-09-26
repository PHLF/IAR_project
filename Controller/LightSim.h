#ifndef LIGHTSIM_H
#define LIGHTSIM_H

#include "../Misc/Globals.h"

#include "LocalThreadSim.h"

namespace sim {
class LightSim {
 public:
  LightSim();
  ~LightSim();

  friend std::ostream& operator<<(std::ostream& os, LightSim const& lightsim);
  friend std::istream& operator>>(std::istream& os, LightSim& lightsim);

  void sim();

 private:
  std::vector<MarkovBrain> _pred_pool;
  std::vector<MarkovBrain> _prey_pool;

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
                                            {"pred_mb_max_inputs", 4},
                                            {"pred_mb_max_outputs", 4},
                                            {"pred_mb_nb_ancestor_genes", 12},
                                            {"prey_speed", 1},
                                            {"prey_turn_speed", 8},
                                            {"prey_los", 100},
                                            {"prey_fov", 180},
                                            {"prey_retina_cells_by_layer", 12},
                                            {"prey_mb_max_inputs", 4},
                                            {"prey_mb_max_outputs", 4},
                                            {"prey_mb_nb_ancestor_genes", 12},
                                            {"pred_generations", 10},
                                            {"pred_pool_size", 64},
                                            {"prey_generations", 400},
                                            {"prey_pool_size", 400},
                                            {"evolve_prey", 0},
                                            {"evolve_pred", 1},
                                            {"predator_confusion", 1}};

  void _setup_sim();
  uint64_t _fitness_proportionate_selection(
      std::map<uint32_t, uint64_t>& fitness_with_seeds);
};
}
#endif  // LIGHTSIM_H
