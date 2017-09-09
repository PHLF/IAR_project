#ifndef AGENT_H
#define AGENT_H

#include "../../Misc/Utils.h"

#include "Retina.h"

namespace sim {
class Agent {
 public:
  int32_t orientation;
  uint32_t speed;
  uint32_t turn_speed;
  Coords coord;
  bool turned_left = false;
  bool turned_right = false;
  uint32_t handling_time = 0;

  Agent(bool predates_,
        uint32_t speed,
        uint32_t turn_speed,
        uint32_t segments,
        float los,
        float fov);
  virtual ~Agent();

  virtual void turn_left();
  virtual void turn_right();
  virtual void observe(Agents const& agents);
  virtual std::vector<uint8_t> get_state();

  bool predates() const;
  Retina const& retina() const;

 protected:
  bool _predates;
  Retina _retina;

  friend std::ostream& operator<<(std::ostream& stream, const Agent& a);
};
}

#endif  // AGENT_H
