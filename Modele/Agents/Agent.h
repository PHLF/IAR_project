#ifndef AGENT_H
#define AGENT_H

#include <Divers/Utils.h>

#include <Modele/Agents/Retina.h>

namespace sim {
class Agent {
 public:
  const bool predates;
  uint32_t orientation;  // Angle in °
  uint32_t speed;
  uint32_t turn_speed;
  Coords coord;

  Agent(bool predates_,
        uint32_t speed_,
        uint32_t turn_speed_,
        uint32_t orientation_,
        uint32_t segments_,
        float view_depth_,
        float fov_);

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
