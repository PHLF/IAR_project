#include "Retina.h"

#include <algorithm>
#include <cmath>

using namespace sim;

RetinaBase::RetinaBase(uint32_t nb_segments,
               uint32_t los_,
               uint32_t fov_,
               const Coords& ref_position)
    : _ref_position(ref_position), _nb_segments(nb_segments), _los(los_) {
  for (uint32_t theta = ((fov_ % 360) / 2); theta > 0;
       theta -= fov_ / nb_segments) {
    _theta_i.emplace_back(theta);
  }
  for (uint32_t theta = 360; theta >= 360 - (fov_ % 361) / 2;
       theta -= fov_ / nb_segments) {
    _theta_i.emplace_back(theta);
  }
  _view_vectors.resize(_theta_i.size());
}

RetinaBase::~RetinaBase() {}

void RetinaBase::update_view_vectors(uint32_t orientation) {
  const auto nb_theta_angles = _theta_i.size();

  for (size_t i = 0; i < nb_theta_angles; ++i) {
    _view_vectors[i].x =
        static_cast<int32_t>(_los * cos(orientation + _theta_i[i]));
    _view_vectors[i].y =
        static_cast<int32_t>(_los * sin(orientation + _theta_i[i]));
  }
}

const std::vector<Coords>& RetinaBase::view_vectors() const {
  return _view_vectors;
}
