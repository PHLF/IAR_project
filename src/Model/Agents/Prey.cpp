#include "Prey.h"

#include <iterator>

using namespace sim;

Prey::Prey(const MarkovBrain& brain_,
           uint32_t speed,
           uint32_t turn_speed,
           uint32_t segments,
           uint32_t los,
           uint32_t fov,
           SDL_Texture* sprite_)
    : Agent(brain_, speed, turn_speed, segments, los, fov, sprite_) {}
Prey::~Prey() {}

std::vector<uint8_t> Prey::get_state() {
  auto input = Agent::get_state();

  input.insert(std::cend(input), std::cbegin(_retina->cells_predators),
               std::cend(_retina->cells_predators));

  return input;
}

void Prey::is_seen(Retina& retina, size_t cell_index) const {
  retina.cells_preys[cell_index] = 1;
}

Color Prey::color() const {
  return {0, 0, 0, 255};
}
