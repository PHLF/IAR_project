#ifndef LIGHTSIM_H
#define LIGHTSIM_H

#include <Divers/Globals.h>

#include <Modele/Agents/Predator.h>
#include <Modele/Agents/Prey.h>
#include <Modele/Environnement/Environment.h>

class LightSim {
 public:
  LightSim(uint32_t x, uint32_t y, uint32_t nbPredators, uint32_t nbPreys);
  virtual ~LightSim();

  bool run(uint32_t);

 protected:
 private:
  uint32_t _tick;
  std::shared_ptr<Environment> _env;
};
#endif  // LIGHTSIM_H
