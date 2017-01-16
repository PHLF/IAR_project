#ifndef LIGHTSIM_H
#define LIGHTSIM_H

#include <Divers/Globals.h>

#include <Modele/Agents/Predator.h>
#include <Modele/Agents/Prey.h>
#include <Modele/Environnement/Environment.h>

class LightSim {
  using Agents = std::vector<std::unique_ptr<Agent>>;

 public:
  LightSim(uint32_t x, uint32_t y, uint32_t nbPredators, uint32_t nbPreys);
  virtual ~LightSim();

  bool run(uint32_t);

 private:
  uint32_t _tick;
  std::unique_ptr<Environment> _env;
  std::default_random_engine _generator;

  void _setup_agents();
  void _print_agents();
  void _move_agents();
  void _observe_agents();

  float _random_x();
  float _random_y();
  uint32_t _random_orientation();
};
#endif  // LIGHTSIM_H
