#ifndef UTILS_H
#define UTILS_H

#include <Divers/Globals.h>

namespace sim {

double sin(uint32_t angle_in_degrees);
double cos(uint32_t angle_in_degrees);

struct Coords {
  double x;
  double y;
};
}

#endif  // UTILS_H
