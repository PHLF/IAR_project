#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../../Misc/Globals.h"

#include "../Model/Agents/Prey.h"
#include "../Model/Agents/Predator.h"

namespace sim {
class Environment {
 public:
  Environment(uint32_t x, uint32_t y);
  virtual ~Environment();

  const uint32_t size_x;
  const uint32_t size_y;
};
}

#endif  // ENVIRONMENT_H
