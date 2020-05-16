#include "layer.h"

#include "fmt/printf.h"

bool sim::LayerBase::_in_circular_sector(const Coords& dist_vec,
                                         const Cell& first_cell,
                                         const Cell& last_cell) {
  return _are_clockwise(first_cell.left_vector, dist_vec) &&
         !_are_clockwise(last_cell.right_vector, dist_vec);
}

void sim::LayerBase::clear() {
  for (auto& cell : _cells) {
    cell.target = nullptr;
  }
  _nb_stimuli = 0;
}

bool sim::LayerBase::_are_clockwise(const Coords& v1, const Coords& v2) {
  return -v1.x * v2.y + v1.y * v2.x > 0;
}
bool sim::LayerBase::_in_range(const Coords& dist_vec) {
  return dist_vec.x * dist_vec.x + dist_vec.y * dist_vec.y <= (_los * _los);
}

void sim::LayerBase::_visit(Agent& agent) {
  const Coords dist_vec = {agent.coords.x - _ref_pos.x,
                           agent.coords.y - _ref_pos.y};

  if (_in_range(dist_vec) &&
      _in_circular_sector(dist_vec, _cells.front(), _cells.back())) {
    for (size_t i = 0; i < _cells.size(); ++i) {
      auto& cell = _cells[i];

      if (_in_circular_sector(dist_vec, cell, cell)) {
        if (cell.target == nullptr) {
       //   fmt::print(
       //       "agent at ({:4.4f},{:4.4f}) seen by agent at ({:4.4f},{:4.4f}) "
       //       "in cell {}\n",
       //       static_cast<double>(agent.coords.x),
       //       static_cast<double>(agent.coords.y),
       //       static_cast<double>(_ref_pos.x), static_cast<double>(_ref_pos.y),
       //       i);
          cell.target = &agent;
          ++_nb_stimuli;
        } else {
          // Keep ref to closest target
          const Coords stored_target_dist_vec = {
              cell.target->coords.x - _ref_pos.x,
              cell.target->coords.y - _ref_pos.y};

          if (dist_vec <= stored_target_dist_vec) {
            cell.target = &agent;
          }
        }
      }
    }
  }
}

size_t sim::LayerBase::nb_stimuli() const {
  return _nb_stimuli;
}

const std::vector<sim::Cell>& sim::LayerBase::cells() const {
  return _cells;
}

template <>
void sim::Layer<sim::Prey>::visit(Prey& prey) {
  _visit(prey);
}

template <>
void sim::Layer<sim::Predator>::visit(Predator& predator) {
  _visit(predator);
}
