#include "Retina.h"

#include <algorithm>
#include <cmath>

using namespace sim;

Retina::Retina(uint32_t seg_, uint32_t los_, uint32_t fov_)
    : cells_predators(seg_, 0),
      cells_preys(seg_, 0),
      _nb_segments(seg_),
      _los(los_) {
  for (uint32_t theta = ((fov_ % 360) / 2); theta > 0; theta -= fov_ / seg_) {
    _theta_i.emplace_back(theta);
  }
  for (uint32_t theta = 360; theta >= 360 - (fov_ % 361) / 2;
       theta -= fov_ / seg_) {
    _theta_i.emplace_back(theta);
  }
  _view_vectors.resize(_theta_i.size());
}

Retina::~Retina() {}

void Retina::update_view_vectors(uint32_t orientation) {
  const auto nb_theta_angles = _theta_i.size();

  for (size_t i = 0; i < nb_theta_angles; ++i) {
    _view_vectors[i].x =
        static_cast<int32_t>(_los * cos(orientation + _theta_i[i]));
    _view_vectors[i].y =
        static_cast<int32_t>(_los * sin(orientation + _theta_i[i]));
  }
}

void Retina::_clear() {
  std::fill(cells_predators.begin(), cells_predators.end(), 0);
  std::fill(cells_preys.begin(), cells_preys.end(), 0);
}

void Retina::see(const Agent& owner, const Agents& agents) {
  _clear();
  for (const auto& agent : agents) {
    if (agent.get() != &owner &&
        _is_inside_sector(owner.coord, *agent, 0, _view_vectors.size() - 1)) {
      for (size_t i = 0; i < _nb_segments; ++i) {
        // Skipping already excitated retina cell
        // FIXME: type info is leaking outside its class through capturable...
        if (agent->capturable() ? cells_preys[i] == 1
                                : cells_predators[i] == 1) {
          continue;
        }
        if (_is_inside_sector(owner.coord, *agent, i, i + 1)) {
          agent->is_seen(*this, i);
        }
      }
    }
  }
}

const std::vector<Coords>& Retina::view_vectors() const {
  return _view_vectors;
}

bool Retina::_are_clockwise(const Coords& v1, const Coords& v2) {
  return -v1.x * v2.y + v1.y * v2.x > 0;
}

bool Retina::_is_in_range(const Coords& dist_vec) {
  return dist_vec.x * dist_vec.x + dist_vec.y * dist_vec.y <= ffloat(_los * _los);
}

bool Retina::_is_inside_sector(const Coords& ref,
                               const Agent& agent,
                               size_t start_index,
                               size_t end_index) {
  const auto& start_sector = _view_vectors[start_index];
  const auto& end_sector = _view_vectors[end_index];

  const Coords dist_vec{agent.coord.x - ref.x, agent.coord.y - ref.y};

  return _is_in_range(dist_vec) && !_are_clockwise(start_sector, dist_vec) &&
         _are_clockwise(end_sector, dist_vec);
}
