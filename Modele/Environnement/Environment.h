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

  uint32_t get_size_x() { return _size_x; };
  uint32_t get_size_y() { return _size_y; };
  Agents& get_agents() { return _agents; };

 private:
  uint32_t _size_x;
  uint32_t _size_y;
  Agents _agents;
};
}

#endif  // ENVIRONMENT_H
