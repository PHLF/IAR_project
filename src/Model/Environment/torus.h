#ifndef TORUS_H
#define TORUS_H

#include "Environment.h"

namespace sim {

class Torus : public Environment {
 public:
  Torus(uint32_t x, uint32_t y);

  void alter(Coords& coords) const override;
};
}  // namespace sim

#endif  // TORUS_H
