#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <Divers/Globals.h>

#include <Modele/Agents/Prey.h>
#include <Modele/Agents/Predator.h>

namespace sim {
class Environment {
 public:
  Environment(uint32_t x, uint32_t y, uint32_t nbPredators, uint32_t nbPreys);
  virtual ~Environment();

  const uint32_t size_x;
  const uint32_t size_y;
  Agents& get_agents() { return _agents; };

 private:
  Agents _agents;
};
}

#endif  // ENVIRONMENT_H
