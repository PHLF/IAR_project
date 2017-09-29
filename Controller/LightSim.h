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
      {"predator_file_fitness_value", 0},
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
    std::map<uint32_t, uint64_t> pred_fitness_with_seeds;
    std::map<uint32_t, uint64_t> prey_fitness_with_seeds;
    std::string sim_output;
  };

  void _setup_sim();
  std::tuple<uint32_t, std::string> _moran_process(
      const std::map<uint32_t, uint64_t>& prey_fit_seeds,
      std::vector<MarkovBrain>& population);

  std::tuple<uint64_t, uint32_t> _fitness_proportionate_selection(
      std::map<uint32_t, uint64_t> fitness_with_seeds,
      bool unfitness = false);
  SimResult _run_thread(uint32_t thread_number,
                        std::vector<MarkovBrain>& pred_pool,
                        std::vector<MarkovBrain>& prey_pool);
};
std::ostream& operator<<(std::ostream& os, LightSim const& lightsim);
std::istream& operator>>(std::istream& os, LightSim& lightsim);
}
#endif  // LIGHTSIM_H
