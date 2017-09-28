#include "Agent.h"

using namespace sim;

Agent::Agent(bool predates_,
             uint32_t speed_,
             uint32_t turn_speed_,
             uint32_t segments_,
             float viewDepth,
             float fov)
    : speed(speed_),
      turn_speed(turn_speed_),
      _predates(predates_),
      _retina(Retina(segments_, viewDepth, fov)) {}

Agent::~Agent() {}

void Agent::turn_left() {
  orientation = (360 + orientation + turn_speed) % 360;
  turned_left = true;
}

void Agent::turn_right() {
  orientation = (360 + orientation - turn_speed) % 360;
  turned_right = true;
}

void Agent::observe(Agents const& agents) {
  Coords sector_start;
  Coords sector_end;
  Agents temp_agents;

  _retina.clear();
  _retina.compute_local_vectors(coord, orientation);

  sector_start = _retina.view_vectors().front();
  sector_end = _retina.view_vectors().back();

  for (auto& agent_j : agents) {
    if (this != &agent_j) {
      if (_retina.is_inside_sector(agent_j.coord, coord, sector_start,
                                   sector_end, _retina.los() * _retina.los())) {
        temp_agents.push_back(agent_j);
      }
    }
  }

  for (const auto& agent_j : temp_agents) {
    for (uint32_t i = 0; i < _retina.get_nb_segments(); ++i) {
      sector_start = _retina.view_vectors()[i];
      sector_end = _retina.view_vectors()[i + 1];

      if (_retina.is_inside_sector(agent_j.coord, coord, sector_start,
                                   sector_end, _retina.los() * _retina.los())) {
        agent_j._predates ? _retina.cells_predators[i] = 1
                          : _retina.cells_preys[i] = 1;
      }
    }
  }
}

std::vector<uint8_t> Agent::get_state() {
  std::vector<uint8_t> input;
  for (auto const cell_prey : _retina.cells_preys) {
    input.push_back(cell_prey);
  }
  if (!_predates) {
    for (auto const cell_pred : _retina.cells_predators) {
      input.push_back(cell_pred);
    }
  }
  input.push_back(turned_left ? 1 : 0);
  input.push_back(turned_right ? 1 : 0);

  return input;
}

bool Agent::predates() const {
  return _predates;
}

Retina const& Agent::retina() const {
  return _retina;
}

std::ostream& sim::operator<<(std::ostream& os, const Agent& a) {
  os << "Speed : " << a.speed;
  os << " turn speed :  " << a.turn_speed;
  os << " orientation : " << a.orientation;
  os << " Coordinates : " << a.coord.x << " " << a.coord.y;
  return os;
}
