#ifndef LIGHTSIM_H
#define LIGHTSIM_H

#include <Misc/Globals.h>

#include <Controller/LocalThreadSim.h>

namespace sim {
class LightSim {
 public:
  LightSim();
  ~LightSim();

  friend std::ostream& operator<<(std::ostream& os, LightSim const& lightsim);
  friend std::istream& operator>>(std::istream& os, LightSim& lightsim);

  void sim();

  void test_pred();

 private:
  uint64_t _seed;
  MarkovBrain _pred_mb_init;
  MarkovBrain _prey_mb_init;
  std::mutex _io_mutex;

  std::map<std::string, std::string> _settings{
      {"headless", "0"},
      {"threads", "1"},
      {"win_w", "768"},
      {"win_h", "768"},
      {"ticks", "2000"},
      {"grid_w", "512"},
      {"grid_h", "512"},
      {"predators", "1"},
      {"preys", "50"},
      {"pred_speed", "1"},
      {"pred_turn_speed", "8"},
      {"pred_los", "100"},
      {"pred_fov", "180"},
      {"pred_retina_cells", "12"},
      {"prey_speed", "1"},
      {"prey_turn_speed", "8"},
      {"prey_los", "100"},
      {"prey_fov", "180"},
      {"prey_retina_cells_by_layer", "12"},
      {"pred_generations", "10"},
      {"pred_children", "10"},
      {"prey_generations", "400"},
      {"prey_children", "400"},
      {"evolve_prey", "0"},
      {"evolve_pred", "1"},
      {"predator_confusion", "1"},
      {"predator_file", ""},
      {"prey_file", ""}};

  void _train_predator();
  void _setup_sim();
  std::array<uint64_t, 2> _select_parents(
      std::map<uint32_t, uint64_t>& fitness_with_seeds);
};
}
#endif  // LIGHTSIM_H
