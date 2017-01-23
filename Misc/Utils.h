﻿#ifndef UTILS_H
#define UTILS_H

#include <Misc/Globals.h>

namespace sim {

double sin(int32_t angle_in_degrees);
double cos(int32_t angle_in_degrees);

struct Coords {
  double x;
  double y;
};

  bool operator==(Coords const& lhs,Coords const& rhs);
  bool is_near(Coords const & a,Coords const & b,double margin);
}


uint32_t convert_vbool_to_uint32(std::vector<bool>& vect);
uint32_t get_vbool_pos(std::vector<bool>& vect);

#endif  // UTILS_H