#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <Divers/Globals.h>

#include <Modele/Agents/Prey.h>
#include <Modele/Agents/Predator.h>

class Environment {
  using Agents = std::vector<std::unique_ptr<Agent>>;

 public:
  Environment(uint32_t x, uint32_t y, uint32_t nbPredators, uint32_t nbPreys);
  virtual ~Environment();

  uint32_t get_size_x() { return _size_x; };
  uint32_t get_size_y() { return _size_y; };
  Agents& get_predators() { return _predators; };
  Agents& get_preys() { return _preys; };

 private:
  uint32_t _size_x;
  uint32_t _size_y;
  Agents _predators;
  Agents _preys;
};

#endif  // ENVIRONMENT_H
