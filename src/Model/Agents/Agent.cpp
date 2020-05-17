#include "Agent.h"

#include "Retina.h"

#include "fmt/printf.h"

using namespace sim;

Agent::Agent(MarkovBrain const& brain_,
             size_t nb_memory_cells,
             uint32_t speed_,
             uint32_t turn_speed_,
             SDL_Texture* sprite_)
    : speed(speed_),
      turn_speed(turn_speed_),
      _ios({{[this] {
               const auto has_turned_left = turned_left;
               turned_left = false;
               return has_turned_left;
             },
             std::bind(&Agent::turn_left, this)},
            {[this] {
               const auto has_turned_right = turned_right;
               turned_right = false;
               return has_turned_right;
             },
             std::bind(&Agent::turn_right, this)},
            {[this] {
               const auto has_moved_forward = moved_forward;
               moved_forward = false;
               return has_moved_forward;
             },
             std::bind(&Agent::forward, this)}}),
      _alive(true),
      _retina(nullptr),
      _brain(brain_),
      _sprite(sprite_) {
  memory_cells.resize(nb_memory_cells);
  memory_cells.shrink_to_fit();

  for (auto& cell : memory_cells) {
    IO memory_io = {[&cell] { return cell; }, [&cell] { cell = true; }};
    _ios.emplace_back(std::move(memory_io));
  }
}

void Agent::setup_sensor_ios() {
  if (_retina != nullptr) {
    auto layers = _retina->layers();
    for (size_t i = 0; i < layers.size; ++i) {
      for (auto& cell : layers.data[i]->cells()) {
        IO retina_io = {[&cell] { return cell.target != nullptr; },
                        [] { /* NOP */ }};
        _ios.emplace_back(std::move(retina_io));
      }
    }
  }
}

Agent::~Agent() {}

size_t Agent::nb_actions() {
  // forward + turn_left + turn_right
  return 3;
}

void Agent::turn_left() {
  orientation = (360 + orientation + turn_speed) % 360;

  turned_left = true;
}

void Agent::turn_right() {
  orientation = (360 + orientation - turn_speed) % 360;

  turned_right = true;
}

void Agent::forward() {
  coords.x += speed * cos(orientation);
  coords.y += speed * sin(orientation);

  moved_forward = true;
}

void Agent::move(Environment const& environment) {
  _brain.actions(_ios);

  environment.alter(coords);
}

void Agent::observe(const std::vector<std::unique_ptr<Agent>>& agents) {
  _retina->clear();
  _retina->update_view_vectors(orientation);

  for (auto& agent : agents) {
    if (agent.get() != this && agent->is_alive()) {
      _retina->see(*agent);
    }
  }
}

void Agent::captures() {}

bool Agent::is_alive() {
  return _alive;
}

RetinaBase const& Agent::retina() const {
  return *_retina;
}

SDL_Texture* Agent::get_sprite() {
  if (_alive) {
    return _sprite;
  }
  return nullptr;
}

void Agent::set_alive(bool value) {
  _alive = value;
}

std::ostream& sim::operator<<(std::ostream& os, const Agent& a) {
  os << "Speed : " << a.speed;
  os << " turn speed :  " << a.turn_speed;
  os << " orientation : " << a.orientation;
  os << " coordinates : " << a.coords.x << " " << a.coords.y;
  return os;
}
