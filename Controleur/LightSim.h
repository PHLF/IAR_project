#ifndef LIGHTSIM_H
#define LIGHTSIM_H

#include <Divers/Globals.h>

#include <Modele/Agents/Predator.h>
#include <Modele/Agents/Prey.h>
#include <Modele/Environnement/Environment.h>
#include <Vue/FenetrePrincipale.h>
#include <Modele/Evo/Mn.h>

namespace sim {
class LightSim {
 public:
  Mn prey_mn;
  Mn pred_mn;

  LightSim(uint32_t win_w,
           uint32_t win_h,
           uint32_t grid_x,
           uint32_t grid_y,
           uint32_t nb_predators,
           uint32_t nb_preys);

  LightSim(uint32_t grid_x,
           uint32_t grid_y,
           uint32_t nb_predators,
           uint32_t nb_preys);

  virtual ~LightSim();

  bool run(uint32_t);

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

 private:
  uint32_t _fitness_predator;
  uint32_t _fitness_prey;
  uint32_t _tick;
  uint32_t _nb_captures;
  std::vector<uint32_t> _preys_alive;
  std::unique_ptr<Environment> _env;
  std::default_random_engine _generator;

  std::unique_ptr<FenetrePrincipale> _fen;

  void _setup_agents();
  void _print_agents();
  void _move_agents();
  void _capture_preys();
  void _observe_agents();
  void _move_agent(Agent& agent);
  void _turn_l_agent(Agent& agent);
  void _turn_r_agent(Agent& agent);
  double _random_x();
  double _random_y();
  uint32_t _random_orientation();

  bool run_ui(uint32_t nbTicks);
  bool run_headless(uint32_t nbTicks);

  bool _areClockwise(Coords v1, Coords v2);
  bool _isWithinRadius(Coords v, uint32_t radiusSquared);
  bool _isInsideSector(Coords point,
                       Coords center,
                       Coords sectorStart,
                       Coords sectorEnd,
                       uint32_t radiusSquared);
};
}
#endif  // LIGHTSIM_H
