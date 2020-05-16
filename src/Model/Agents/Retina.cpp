#include "Retina.h"

#include <algorithm>

using namespace sim;

RetinaBase::RetinaBase(uint32_t nb_segments,
                       uint32_t los_,
                       uint32_t fov_,
                       const Coords& ref_position)
    : _ref_position(ref_position), _nb_segments(nb_segments), _los(los_) {
  const int32_t fov = static_cast<int32_t>(fov_) > 360 ? 360 : fov_;
  const int32_t step = fov / nb_segments;

  for (int32_t theta = -fov / 2; theta <= fov / 2; theta += step) {
    _theta_i.emplace_back((360 + theta) % 360);
  }

  _view_vectors.resize(_theta_i.size());
}

RetinaBase::~RetinaBase() {}

void RetinaBase::update_view_vectors(uint32_t orientation) {
  const auto nb_view_vectors = _view_vectors.size();
  for (size_t i = 0; i < nb_view_vectors; ++i) {
    _view_vectors[i].x = _los * cos(orientation + _theta_i[i]);
    _view_vectors[i].y = _los * sin(orientation + _theta_i[i]);
  }
}

const std::vector<Coords>& RetinaBase::view_vectors() const {
  return _view_vectors;
}
