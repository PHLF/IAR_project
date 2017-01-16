#ifndef LIGHTSIM_H
#define LIGHTSIM_H

#include <random>
#include <chrono>

#include <Divers/Globals.h>

#include <Modele/Agents/Predator.h>
#include <Modele/Agents/Prey.h>
#include <Modele/Agents/Agent.h>
#include <Modele/Environnement/Environment.h>


class LightSim {
 public:
  LightSim(uint32_t x, uint32_t y, uint32_t nbPredators, uint32_t nbPreys);
  virtual ~LightSim();
  bool run(uint32_t);




 protected:
 private:

  std::default_random_engine generator;
  void setup_agents(std::vector<std::unique_ptr<Agent>>& agents);
  void print_agents(std::vector<std::unique_ptr<Agent>>& agents);
  float random_x();
  float random_y();
  uint32_t random_orientation();
  void move_agents(std::vector<std::unique_ptr<Agent>>& agents);
  void observe_agents(std::vector<std::unique_ptr<Agent>>& agents);
  uint32_t _tick;
  std::shared_ptr<Environment> _env;
};
#endif  // LIGHTSIM_H
