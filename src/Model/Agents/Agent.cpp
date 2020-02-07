#include "Agent.h"

using namespace sim;

Agent::Agent(MarkovBrain const& brain_,
             uint32_t speed_,
             uint32_t turn_speed_,
             uint32_t segments_,
             float viewDepth,
             uint32_t fov,
             SDL_Texture* sprite_)
    : speed(speed_),
      turn_speed(turn_speed_),
      _retina(Retina(segments_, viewDepth, fov)),
      _brain(brain_),
      sprite(sprite_) {}

Agent::~Agent() {}

void Agent::turn_left() {
  orientation = (360 + orientation + turn_speed) % 360;
  turned_left = true;
}

void Agent::turn_right() {
  orientation = (360 + orientation - turn_speed) % 360;
  turned_right = true;
}

void Agent::move(Environment const& environment) {
  std::vector<uint8_t> output;

  output = _brain.actions(get_state());

  if (output[0] != 0) {
    turn_left();
  } else {
    turned_left = false;
  }
  if (output[1] != 0) {
    turn_right();
  } else {
    turned_right = false;
  }

  coord.x += speed * cos(orientation);
  coord.y += speed * sin(orientation);

  environment.alter(coord);
}

void Agent::observe(Agents const& agents) {
  Coords sector_start;
  Coords sector_end;
  std::vector<Agent const*> temp_agents;

  _retina.clear();
  _retina.compute_local_vectors(orientation);

  sector_start = _retina.view_vectors().front();
  sector_end = _retina.view_vectors().back();

  for (auto& agent_j : agents) {
    if (this != &(*agent_j)) {
      if (_retina.is_inside_sector(agent_j->coord, coord, sector_start,
                                   sector_end, _retina.los() * _retina.los())) {
        temp_agents.emplace_back(agent_j.get());
      }
    }
  }

  for (const auto agent_j : temp_agents) {
    for (uint32_t i = 0; i < _retina.get_nb_segments(); ++i) {
      sector_start = _retina.view_vectors()[i];
      sector_end = _retina.view_vectors()[i + 1];

      if (_retina.is_inside_sector(agent_j->coord, coord, sector_start,
                                   sector_end, _retina.los() * _retina.los())) {
        agent_j->is_seen(_retina, i);
      }
    }
  }
}

bool Agent::captures(const Agent& /*agent*/) {
  return false;
}

bool Agent::capturable() const {
  return true;
}

std::vector<uint8_t> Agent::get_state() {
  std::vector<uint8_t> input;

  input = _retina.cells_preys;

  input.push_back(turned_left ? 1 : 0);
  input.push_back(turned_right ? 1 : 0);

  return input;
}

Retina const& Agent::retina() const {
  return _retina;
}

SDL_Texture* Agent::get_sprite() {
  return sprite;
}

std::ostream& sim::operator<<(std::ostream& os, const Agent& a) {
  os << "Speed : " << a.speed;
  os << " turn speed :  " << a.turn_speed;
  os << " orientation : " << a.orientation;
  os << " Coordinates : " << a.coord.x << " " << a.coord.y;
  return os;
}
