#ifndef AGENT_H
#define AGENT_H

#include "Retina.h"
#include <Modele/Environnement/utils.h>

class Agent {
 public:
  Agent(sim::Environment* env,
        uint32_t speed = 1,
        uint32_t turnSpeed = 8,
        uint32_t orientation = 0,
        float viewDepth = 100,
        float fov = 180);
  virtual ~Agent();
  virtual void moveForward();
  virtual void turnLeft();
  virtual void turnRight();
  virtual void observe();
  virtual bool run();

 protected:
  // Environment _env;
  uint32_t _speed;
  uint32_t _turnSpeed;
  uint32_t _orientation;  // Angle in °
  coord_s _coord;
  std::unique_ptr<Retina> retina;
  std::vector<bool> _observationPredators;
  std::vector<bool> _observationPreys;

 private:
};

#endif  // AGENT_H
