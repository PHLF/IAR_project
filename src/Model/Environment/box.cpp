#include "box.h"

sim::Box::Box(uint32_t x, uint32_t y) : Environment(x, y) {}

void sim::Box::alter(sim::Coords& coords) const {
  if (coords.x > size_x) {
    coords.x = size_x;
  } else if (coords.x < 0) {
    coords.x = 0;
  }
  if (coords.y > size_y) {
    coords.y = size_y;
  } else if (coords.y < 0) {
    coords.y = 0;
  }
}
