#include "Retina.h"

#include <cmath>
#include <algorithm>

using namespace sim;

Retina::Retina(uint32_t seg_, float los_, float fov_)
    : cells_predators(seg_, 0),
      cells_preys(seg_, 0),
      _nb_segments(seg_),
      _field_of_view(fov_),
      _los(los_) {
  for (int32_t theta = std::roundf(-fov_ / 2); theta <= fov_ / 2;
       theta += std::roundf(fov_ / seg_)) {
    _theta_i.emplace_back(theta);
  }
}

Retina::~Retina() {}

const std::vector<Coords>& Retina::view_vectors() const {
  return _view_vectors;
}

void Retina::compute_local_vectors(Coords current_pos, uint32_t orientation) {
  Coords local_view_vector;
  _view_vectors.clear();
  for (const auto& theta : _theta_i) {
    local_view_vector.x = _los * cos(static_cast<int32_t>(orientation) + theta);
    local_view_vector.y = _los * sin(static_cast<int32_t>(orientation) + theta);
    _view_vectors.push_back(local_view_vector);
  }
}

void Retina::clear() {
  std::fill(cells_predators.begin(), cells_predators.end(), 0);
  std::fill(cells_preys.begin(), cells_preys.end(), 0);
}

float Retina::los() const {
  return _los;
}

bool Retina::_are_clockwise(Coords v1, Coords v2) {
  return -v1.x * v2.y + v1.y * v2.x > 0;
}

bool Retina::_is_within_radius(Coords v, uint32_t radiusSquared) {
  return v.x * v.x + v.y * v.y <= radiusSquared;
}

bool Retina::is_inside_sector(Coords point,
                              Coords center,
                              Coords sectorStart,
                              Coords sectorEnd,
                              uint32_t radiusSquared) {
  Coords relPoint{point.x - center.x, point.y - center.y};

  return !_are_clockwise(sectorStart, relPoint) &&
         _are_clockwise(sectorEnd, relPoint) &&
         _is_within_radius(relPoint, radiusSquared);
}
