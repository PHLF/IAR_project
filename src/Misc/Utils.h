#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

namespace sim {

double sin(int32_t angle_in_degrees);
double cos(int32_t angle_in_degrees);

struct Coords {
  double x;
  double y;
};

bool operator==(Coords const& lhs, Coords const& rhs);
bool is_near(Coords const& a, Coords const& b, double margin);
}

#endif  // UTILS_H
