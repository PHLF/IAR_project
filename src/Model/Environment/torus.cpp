#include "torus.h"

sim::Torus::Torus(uint32_t x, uint32_t y) : Environment(x, y) {}

void sim::Torus::alter(sim::Coords& coords) const {
  if (coords.x > size_x) {
    coords.x = 0;
  } else if (coords.x < 0) {
    coords.x = size_x;
  }

  if (coords.y > size_y) {
    coords.y = 0;
  } else if (coords.y < 0) {
    coords.y = size_y;
  }
}
