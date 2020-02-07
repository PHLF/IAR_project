#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

namespace sim {

double sin(uint32_t angle);
double cos(uint32_t angle);

struct Coords {
  double x;
  double y;
};

bool operator==(Coords const& lhs, Coords const& rhs);
bool is_near(Coords const& a, Coords const& b, double margin);
}

#endif  // UTILS_H
