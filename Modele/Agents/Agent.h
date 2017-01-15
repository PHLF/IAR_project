#ifndef AGENT_H
#define AGENT_H

#include <Divers/Globals.h>

#include <Modele/Agents/Retina.h>
#include <Modele/Environnement/utils.h>

class Agent {
 public:
  Agent(uint32_t speed = 1,
        uint32_t turnSpeed = 8,
        uint32_t orientation = 0,
        uint32_t segments = 7,
        float viewDepth = 100,
        float fov = 180);
  virtual ~Agent();

  virtual void moveForward();
  virtual void turnLeft();
  virtual void turnRight();
  virtual void observe();
  virtual bool run();

 protected:
  uint32_t _speed;
  uint32_t _turnSpeed;
  uint32_t _orientation;  // Angle in °

  coord_s _coord;
  std::unique_ptr<Retina> _retina;

 private:
};

#endif  // AGENT_H
