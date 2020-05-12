#ifndef LAYER_H
#define LAYER_H

#include <vector>

#include "AgentVisitor.h"
#include "Misc/Utils.h"
#include "Predator.h"
#include "Prey.h"

namespace sim {

struct Cell {
  Agent* target;
  const Coords& left_vector;
  const Coords& right_vector;
};

class LayerBase : public AgentVisitor {
 public:
  LayerBase(const Coords& ref_position,
            const std::vector<Coords> view_vectors_,
            ffloat los_)
      : _los(los_), _ref_pos(ref_position) {
    const auto nb_view_vectors = view_vectors_.size();

    for (size_t i = 0; i < nb_view_vectors; ++i) {
      Cell cell{nullptr, view_vectors_[i], view_vectors_[i + 1]};
      _cells.emplace_back(std::move(cell));
    }
  }

  void clear();

  const std::vector<Cell>& cells() const;

  size_t nb_stimuli() const;

 protected:
  // http://stackoverflow.com/questions/13652518/efficiently-find-points-inside-a-circle-sector.
  static bool _are_clockwise(const Coords& v1, const Coords& v2);
  bool _in_circular_sector(const Coords& dist_vec,
                           const Cell& first_cell,
                           const Cell& last_cell);
  bool _in_range(const Coords& dist_vec);
  void _visit(Agent& agent);

  std::vector<Cell> _cells;

  size_t _nb_stimuli;
  ffloat _los;
  const Coords& _ref_pos;
};

template <typename StimulusType>
class Layer : public LayerBase {
  using LayerBase::LayerBase;

 public:
  void visit(Prey& /* prey */) override {}
  void visit(Predator& /* predator */) override {}
};

}  // namespace sim

#endif  // LAYER_H
