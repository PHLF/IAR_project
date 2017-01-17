#include <Modele/Agents/Retina.h>

using namespace sim;

Retina::Retina(uint32_t seg_, double depth_, double fov_)
    : cells_predators(seg_, false),
      cells_preys(seg_, false),
      _nb_segments(seg_),
      _field_of_view(fov_),
      _depth(depth_) {
  for (int32_t theta = std::roundf(-fov_ / 2); theta <= fov_ / 2;
       theta += std::roundf(fov_ / seg_)) {
    _theta_i.emplace_back(theta);
  }
}

Retina::~Retina() {
  // dtor
}

const std::vector<Coords>& Retina::get_view_vectors() const {
  return _view_vectors;
}

void Retina::compute_local_vectors(Coords current_pos, uint32_t orientation) {
  Coords local_view_vector;
  for (const auto& theta : _theta_i) {
    local_view_vector.x =
        _depth * cos(static_cast<int32_t>(orientation) + theta);
    local_view_vector.y =
        _depth * sin(static_cast<int32_t>(orientation) + theta);
    _view_vectors.push_back(local_view_vector);
  }
}

void Retina::clear() {
  std::fill(cells_predators.begin(), cells_predators.end(), false);
  std::fill(cells_preys.begin(), cells_preys.end(), false);
}

float Retina::getDepth() const {
  return _depth;
}
