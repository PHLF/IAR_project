#include "Agent.h"

#include "Retina.h"

using namespace sim;

Agent::Agent(MarkovBrain const& brain_,
             uint32_t speed_,
             uint32_t turn_speed_,
             SDL_Texture* sprite_)
    : speed(speed_),
      turn_speed(turn_speed_),
      _ios({{false, std::bind(&Agent::turn_left, this)},
            {false, std::bind(&Agent::turn_right, this)},
            {false, std::bind(&Agent::forward, this)}}),
      _retina(nullptr),
      _brain(brain_),
      _sprite(sprite_) {}

Agent::~Agent() {}

void Agent::turn_left() {
  orientation = (360 + orientation + turn_speed) % 360;
}

void Agent::turn_right() {
  orientation = (360 + orientation - turn_speed) % 360;
}

void Agent::forward() {
  coords.x += speed * cos(orientation);
  coords.y += speed * sin(orientation);
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

  environment.alter(coords);
}

void Agent::observe(const std::vector<std::unique_ptr<Agent>>& agents) {
  _retina->clear();
  _retina->update_view_vectors(orientation);

  for (auto& agent : agents) {
    _retina->see(*agent);
  }
}

void Agent::captures() {}

std::vector<uint8_t> Agent::get_state() {
  std::vector<uint8_t> input;

  // input = _retina->cells_preys;

  input.push_back(turned_left ? 1 : 0);
  input.push_back(turned_right ? 1 : 0);

  return input;
}

RetinaBase const& Agent::retina() const {
  return *_retina;
}

SDL_Texture* Agent::get_sprite() {
  return _sprite;
}

std::ostream& sim::operator<<(std::ostream& os, const Agent& a) {
  os << "Speed : " << a.speed;
  os << " turn speed :  " << a.turn_speed;
  os << " orientation : " << a.orientation;
  os << " coordinates : " << a.coords.x << " " << a.coords.y;
  return os;
}
