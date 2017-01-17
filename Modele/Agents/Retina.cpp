#include <Modele/Agents/Retina.h>

using namespace sim;

Retina::Retina(uint32_t seg, float depth, float fov)
    : _nb_segments(seg),
      _field_of_view(fov),
      _depth(depth),
      _cells_predators(seg, false),
      _cells_preys(seg, false) {
  for (int32_t theta = std::roundf(-fov / 2); theta <= fov / 2;
       theta += std::roundf(fov / seg)) {
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
