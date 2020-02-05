#ifndef BOX_H
#define BOX_H

#include "Environment.h"

namespace sim {

class Box : public Environment {
 public:
  Box(uint32_t x, uint32_t y);

  void alter(Coords& coords) const override;
};
}  // namespace sim

#endif  // BOX_H
