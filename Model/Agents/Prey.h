#ifndef PREY_H
#define PREY_H

#include "Agent.h"

namespace sim {
class Prey : public Agent {
 public:
  Prey(uint32_t speed,
       uint32_t turn_speed,
       uint32_t segments,
       float los,
       float fov);
  ~Prey();
};
}

#endif  // PREY_H
