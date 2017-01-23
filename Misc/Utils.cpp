﻿#include <Misc/Utils.h>

double sim::sin(int32_t angle_in_degrees) {
  static std::array<double, 91> sin{
      {0,        0.017452, 0.034899, 0.052336, 0.069756, 0.087156, 0.104528,
       0.121869, 0.139173, 0.156434, 0.173648, 0.190809, 0.207912, 0.224951,
       0.241922, 0.258819, 0.275637, 0.292372, 0.309017, 0.325568, 0.34202,
       0.358368, 0.374607, 0.390731, 0.406737, 0.422618, 0.438371, 0.45399,
       0.469472, 0.48481,  0.5,      0.515038, 0.529919, 0.544639, 0.559193,
       0.573576, 0.587785, 0.601815, 0.615661, 0.62932,  0.642788, 0.656059,
       0.669131, 0.681998, 0.694658, 0.707107, 0.71934,  0.731354, 0.743145,
       0.75471,  0.766044, 0.777146, 0.788011, 0.798636, 0.809017, 0.819152,
       0.829038, 0.838671, 0.848048, 0.857167, 0.866025, 0.87462,  0.882948,
       0.891007, 0.898794, 0.906308, 0.913545, 0.920505, 0.927184, 0.93358,
       0.939693, 0.945519, 0.951057, 0.956305, 0.961262, 0.965926, 0.970296,
       0.97437,  0.978148, 0.981627, 0.984808, 0.987688, 0.990268, 0.992546,
       0.994522, 0.996195, 0.997564, 0.99863,  0.999391, 0.999848, 1}};

  int32_t angle = 0;
  double signe = 1.0;

  angle = angle_in_degrees % 360;
  if (angle < 0) {
    angle = 360 + angle;
  }

  if (angle > 90) {
    if (angle <= 180) {
      angle = 180 - angle;
    } else if (angle > 180 && angle <= 270) {
      angle = angle % 180;
      signe = -1.0;
    } else if (angle > 270 && angle <= 360) {
      angle = angle - 180;
      signe = -1.0;
    }
    return signe * sim::sin(angle);
  }
  return sin[angle];
}

double sim::cos(int32_t angle_in_degrees) {
  return sim::sin(angle_in_degrees + 90);
}

bool sim::operator==(const sim::Coords& lhs, const sim::Coords& rhs) {
  return std::round(lhs.x) == std::round(rhs.x) &&
         std::round(lhs.y) == std::round(rhs.y);
}

bool sim::is_near(sim::Coords const& a, sim::Coords const& b, double margin) {
  return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2)) < margin;
}

uint32_t convert_vbool_to_uint32(std::vector<bool>& vect) {
  uint32_t integer = 0, n = vect.size() - 1;
  for (auto i : vect) {
    if (i) {
      integer += 1 << n;
    }
    n = n - 1;
  }
  return integer;
}

uint32_t get_vbool_pos(std::vector<bool>& vect) {
  uint32_t integer = 0, n = vect.size() - 1;
  for (auto i : vect) {
    if (i) {
      integer = n;
    }
    n -= 1;
  }
  return integer;
}