#ifndef RETINA_H
#define RETINA_H

#include <Misc/Utils.h>

namespace sim {

class Retina {
 public:
  Retina(uint32_t seg, double view_depth, double fov);
  ~Retina();

  std::vector<uint8_t> cells_predators;
  std::vector<uint8_t> cells_preys;

  const std::vector<Coords>& get_view_vectors() const;
  uint32_t get_nb_segments() const { return _nb_segments; }
  float getDepth() const;

  void compute_local_vectors(Coords current_pos, uint32_t orientation);
  void clear();

 private:
  uint32_t _nb_segments;
  double _field_of_view;
  double _depth;

  std::vector<double> _theta_i;
  std::vector<Coords> _view_vectors;
};
}

#endif  // RETINA_H
