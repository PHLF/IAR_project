#include "Retina.h"

#include <cmath>
#include <algorithm>

using namespace sim;

Retina::Retina(uint32_t seg_, float los_, uint32_t fov_)
    : cells_predators(seg_, 0),
      cells_preys(seg_, 0),
      _nb_segments(seg_),
      _los(los_) {
  for (uint32_t theta = ((fov_ % 360) / 2); theta > 0;
         theta -= fov_ / seg_ ) {
    _theta_i.emplace_back(theta);
  }
  for (uint32_t theta = 360; theta >= 360 - (fov_ % 361) / 2;
         theta -= fov_ / seg_ ) {
    _theta_i.emplace_back(theta);
  }
  _view_vectors.resize(_theta_i.size());
}

Retina::~Retina() {}

const std::vector<Coords>& Retina::view_vectors() const {
  return _view_vectors;
}

void Retina::compute_local_vectors(uint32_t orientation) {
    const auto nb_theta_angles = _theta_i.size();

  for(size_t i =0; i < nb_theta_angles; ++i){
      _view_vectors[i].x = _los * cos(orientation + _theta_i[i]);
      _view_vectors[i].y = _los * sin(orientation + _theta_i[i]);
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
