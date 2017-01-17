#ifndef RETINA_H
#define RETINA_H

#include <Divers/Utils.h>

namespace sim {
/**
 * @brief The Retina class bringing visual information to an agent. See
 * http://stackoverflow.com/questions/13652518/efficiently-find-points-inside-a-circle-sector.
 */
class Retina {
 public:
  Retina(uint32_t seg, float view_depth, float fov);
  ~Retina();

  std::vector<bool> cells_predators;
  std::vector<bool> cells_preys;

  const std::vector<Coords>& get_view_vectors() const;
  uint32_t get_nb_segments() const { return _nb_segments; }
  float getDepth() const;

  void compute_local_vectors(Coords current_pos, uint32_t orientation);
  void clear();

 private:
  uint32_t _nb_segments;
  float _field_of_view;  // Angle in degrees
  float _depth;

  std::vector<double> _theta_i;
  std::vector<Coords> _view_vectors;
};
}

#endif  // RETINA_H
