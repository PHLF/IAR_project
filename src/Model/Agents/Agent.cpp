#include "Agent.h"

using namespace sim;

Agent::Agent(MarkovBrain const& brain_,
             uint32_t speed_,
             uint32_t turn_speed_,
             uint32_t segments_,
             uint32_t los,
             uint32_t fov,
             SDL_Texture* sprite_)
    : speed(speed_),
      turn_speed(turn_speed_),
      _retina(new Retina(segments_, los, fov)),
      _brain(brain_),
      sprite(sprite_) {}

Agent::~Agent() {}

void Agent::turn_left() {
  orientation = (360 + orientation + turn_speed) % 360;
}

void Agent::turn_right() {
  orientation = (360 + orientation - turn_speed) % 360;
}

void Agent::move(Environment const& environment) {
  std::vector<uint8_t> output;

  output = _brain.actions(get_state());

  if (output[0] != 0) {
    turn_left();
    turned_left = true;
  } else {
    turned_left = false;
  }
  if (output[1] != 0) {
    turn_right();
    turned_right = true;
  } else {
    turned_right = false;
  }
  if (turned_left && turned_right) {
    turned_left = false;
    turned_right = false;
  }

  coord.x += speed * cos(orientation);
  coord.y += speed * sin(orientation);

  environment.alter(coord);
}

void Agent::observe(const std::vector<std::unique_ptr<Agent> >& agents) {
  _retina->update_view_vectors(orientation);
  _retina->see(*this, agents);
}

bool Agent::captures(const Agent& /*agent*/) {
  return false;
}

bool Agent::capturable() const {
  return true;
}

std::vector<uint8_t> Agent::get_state() {
  std::vector<uint8_t> input;

  input = _retina->cells_preys;

  input.push_back(turned_left ? 1 : 0);
  input.push_back(turned_right ? 1 : 0);

  return input;
}

Retina const& Agent::retina() const {
  return *_retina;
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
