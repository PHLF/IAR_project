#include <Modele/Agents/Retina.h>

Retina::Retina(uint32_t seg, float depth, float fov)
    : _nb_segments(seg),
      _field_of_view(fov),
      _depth(depth),
      _cells_predators(seg, false),
      _cells_preys(seg, false) {
  // ctor
}

Retina::~Retina() {
  // dtor
}
