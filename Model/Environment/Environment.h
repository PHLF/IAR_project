#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <Misc/Globals.h>

#include <Model/Agents/Prey.h>
#include <Model/Agents/Predator.h>

namespace sim {
class Environment {
 public:
  Environment(uint32_t x, uint32_t y, uint32_t nbPredators, uint32_t nbPreys);
  virtual ~Environment();

  void reset();

  const uint32_t size_x;
  const uint32_t size_y;
  Agents& get_agents() { return _agents; };

  uint32_t get_nb_predators() const;
  uint32_t get_nb_preys() const;

 private:
  uint32_t _nb_predators;
  uint32_t _nb_preys;
  Agents _agents;
};
}

#endif  // ENVIRONMENT_H
