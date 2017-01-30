#ifndef PREDATOR_H
#define PREDATOR_H

#include <Model/Agents/Agent.h>

namespace sim {
class Predator : public Agent {
 public:
  Predator(uint32_t speed,
           uint32_t turn_speed,
           uint32_t segments,
           float los,
           float fov,
           bool confusion = true);
  ~Predator();

  bool captures(const Agent& prey);

 private:
  bool _visual_confusion;
};
}

#endif  // PREDATOR_H
