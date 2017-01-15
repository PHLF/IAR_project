#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <Divers/Globals.h>
#include <Modele/Agents/Agent.h>

namespace sim {
class Environment {
 public:
  Environment(uint32_t x, uint32_t y, uint32_t nbPredators, uint32_t nbPreys);
  virtual ~Environment();
  void moveForward();
  void turnLeft();
  void turnRight();
  void observe();
  // coord_s randomCoord();

 protected:
 private:
  uint32_t _size_x;
  uint32_t _size_y;
  std::vector<std::unique_ptr<Agent>> _predators;
  std::vector<std::unique_ptr<Agent>> _preys;
};
}
#endif  // ENVIRONMENT_H
