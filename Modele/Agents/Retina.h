#ifndef RETINA_H
#define RETINA_H

#include <Divers/Globals.h>

namespace sim {
class Retina {
 public:
  Retina(uint32_t seg, double view_depth, double fov);
  ~Retina();

  uint32_t get_nb_segments() const { return _nb_segments; }

 private:
  uint32_t _nb_segments;
  double _field_of_view;  // Angle in degrees
  double _depth;

  std::vector<bool> _cells_predators;
  std::vector<bool> _cells_preys;
};
}

#endif  // RETINA_H
