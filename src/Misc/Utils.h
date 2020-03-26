#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

#include "cnl/fixed_point.h"

namespace sim {

using ffloat = cnl::fixed_point<int64_t, -15>;

ffloat sin(uint32_t angle);
ffloat cos(uint32_t angle);

template <typename T>
struct Span {
  T* data;
  size_t size;
};

struct Coords {
  ffloat x;
  ffloat y;
};

bool operator==(Coords const& lhs, Coords const& rhs);
bool operator<=(Coords const& lhs, Coords const& rhs);
bool is_near(Coords const& a, Coords const& b, ffloat margin);

}  // namespace sim

#endif  // UTILS_H
