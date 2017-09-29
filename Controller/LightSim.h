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

  std::map<std::string, uint32_t> _settings{
      {"headless", 0},
      {"threads", 4},
      {"win_w", 768},
      {"win_h", 768},
      {"ticks", 2000},
      {"torus", 1},
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
      {"generations", 10},
      {"pool_size", 64},
      {"evolve_prey", 0},
      {"evolve_pred", 1},
      {"predator_confusion", 0},
      {"predator_file_seed_value", 0},
      {"prey_file_fitness_value", 0},
      {"proba_site_copy", 25},
      {"proba_site_del", 50},
      {"proba_site_insert", 25},
      {"proba_site_replaced", 50},
      {"proba_site_gaussian_mutation", 50},
      {"proba_gene_duplication", 5},
      {"proba_gene_deletion", 10},
      {"proba_new_gene_insert", 5}};

  struct SimResult {
    std::unordered_map<uint64_t, uint32_t> pred_seeds_with_fitness;
    std::unordered_map<uint64_t, uint32_t> prey_seeds_with_fitness;
    std::string sim_output;
  };

  void _setup_sim();
  void _moran_process(
      std::unordered_map<uint64_t, uint32_t> const& mb_seeds_with_fit,
      std::vector<MarkovBrain>& population);

  uint64_t _stochastic_acceptance(
      std::unordered_map<uint64_t, uint32_t> seeds_with_fitness);
  SimResult _run_thread(uint32_t thread_number,
                        uint32_t generation,
                        std::vector<MarkovBrain>& pred_pool,
                        std::vector<MarkovBrain>& prey_pool);
};
std::ostream& operator<<(std::ostream& os, LightSim const& lightsim);
std::istream& operator>>(std::istream& os, LightSim& lightsim);
}
#endif  // LIGHTSIM_H
