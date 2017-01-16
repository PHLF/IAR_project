#ifndef LIGHTSIM_H
#define LIGHTSIM_H

#include <Divers/Globals.h>

#include <Modele/Agents/Predator.h>
#include <Modele/Agents/Prey.h>
#include <Modele/Environnement/Environment.h>
#include <Vue/FenetrePrincipale.h>

namespace sim {
class LightSim {
 public:
  explicit LightSim(uint32_t win_w,
                    uint32_t win_h,
                    uint32_t grid_x,
                    uint32_t grid_y,
                    uint32_t nb_predators,
                    uint32_t nb_preys);
  virtual ~LightSim();

  bool run(uint32_t);

 private:
  uint32_t _tick;

  std::unique_ptr<Environment> _env;
  std::default_random_engine _generator;

  std::unique_ptr<FenetrePrincipale> _fen;

  void _setup_agents();
  void _print_agents();
  void _move_agents();
  void _observe_agents();

  float _random_x();
  float _random_y();
  uint32_t _random_orientation();
};
}
#endif  // LIGHTSIM_H
