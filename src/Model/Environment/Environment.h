#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <cstdint>

#include "Misc/Utils.h"

namespace sim {
class Environment {
 public:
  Environment(uint32_t x, uint32_t y);
  virtual ~Environment();

  virtual void alter(Coords& coords) const = 0;

  const uint32_t size_x;
  const uint32_t size_y;
};
}  // namespace sim

#endif  // ENVIRONMENT_H
