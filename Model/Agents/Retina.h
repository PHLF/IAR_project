#ifndef RETINA_H
#define RETINA_H

#include <Misc/Utils.h>

namespace sim {

class Retina {
 public:
  Retina(uint32_t seg, float los, float fov);
  ~Retina();

  std::vector<uint8_t> cells_predators;
  std::vector<uint8_t> cells_preys;

  const std::vector<Coords>& view_vectors() const;
  uint32_t get_nb_segments() const { return _nb_segments; }
  float los() const;

  void compute_local_vectors(Coords current_pos, uint32_t orientation);
  void clear();

  bool is_inside_sector(Coords point,
                        Coords center,
                        Coords sectorStart,
                        Coords sectorEnd,
                        uint32_t radiusSquared);

 private:
  uint32_t _nb_segments;
  float _field_of_view;
  float _los;

  std::vector<double> _theta_i;
  std::vector<Coords> _view_vectors;

  // http://stackoverflow.com/questions/13652518/efficiently-find-points-inside-a-circle-sector.
  bool _are_clockwise(Coords v1, Coords v2);
  bool _is_within_radius(Coords v, uint32_t radiusSquared);
};
}

#endif  // RETINA_H
