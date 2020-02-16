#ifndef RETINA_H
#define RETINA_H

#include <cstdint>
#include <vector>

#include "Agent.h"
#include "Misc/Utils.h"
#include "agents.h"

namespace sim {

class Retina {
 public:
  Retina(uint32_t seg, uint32_t los, uint32_t fov);
  ~Retina();

  std::vector<uint8_t> cells_predators;
  std::vector<uint8_t> cells_preys;

  void update_view_vectors(uint32_t orientation);
  void see(const Agent& owner, Agents const& agents);
  const std::vector<Coords>& view_vectors() const;

 private:
  uint32_t _nb_segments;
  ffloat _los;

  std::vector<uint32_t> _theta_i;
  std::vector<Coords> _view_vectors;

  // http://stackoverflow.com/questions/13652518/efficiently-find-points-inside-a-circle-sector.
  bool _are_clockwise(const Coords& v1, const Coords& v2);
  bool _is_in_range(const Coords& dist_vec);
  void _clear();

  bool _is_inside_sector(const Coords& dist_vec,
                         size_t start_index,
                         size_t end_index);
};
}  // namespace sim

#endif  // RETINA_H
