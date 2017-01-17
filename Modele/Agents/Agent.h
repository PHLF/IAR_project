#ifndef AGENT_H
#define AGENT_H

#include <Divers/Globals.h>

#include <Modele/Agents/Retina.h>
#include <Modele/Environnement/utils.h>

namespace sim {
class Agent {
 public:
  const bool predates;
  uint32_t orientation;  // Angle in °
  uint32_t speed;
  uint32_t turn_speed;
  Coords coord;

  Agent(bool predates_ = false,
        uint32_t speed = 1,
        uint32_t turnSpeed = 8,
        uint32_t orientation = 0,
        uint32_t segments = 7,
        double viewDepth = 100,
        double fov = 180);
  virtual ~Agent();

  virtual std::unique_ptr<Retina>& get_retina() { return _retina; };

  virtual void turnLeft();
  virtual void turnRight();

 protected:
  std::unique_ptr<Retina> _retina;

  friend std::ostream& operator<<(std::ostream& stream, const Agent& a);
};
}

#endif  // AGENT_H
