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
  bool turned_left;
  bool turned_right;
  uint32_t handling_time;

  Agent(bool predates_,
        uint32_t speed,
        uint32_t turnSpeed,
        uint32_t orientation,
        uint32_t segments,
        float viewDepth,
        float fov);
  virtual ~Agent();

  virtual std::unique_ptr<Retina>& get_retina() { return _retina; };

  virtual void turnLeft();
  virtual void turnRight();
  virtual std::vector<uint8_t> get_input();

 protected:
  std::unique_ptr<Retina> _retina;

  friend std::ostream& operator<<(std::ostream& stream, const Agent& a);
};
}

#endif  // AGENT_H
