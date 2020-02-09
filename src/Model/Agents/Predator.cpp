#include "Predator.h"

using namespace sim;

Predator::Predator(const MarkovBrain& brain_,
                   uint32_t speed,
                   uint32_t turn_speed,
                   uint32_t segments,
                   uint32_t los,
                   uint32_t fov,
                   bool confusion,
                   SDL_Texture* sprite_)
    : Agent(brain_, speed, turn_speed, segments, los, fov, sprite_),
      _visual_confusion(confusion) {}

Predator::~Predator() {}

bool Predator::captures(const Agent& agent) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<uint8_t> d_cap(0, 100);

  if (handling_time > 0) {
    --handling_time;
    return false;
  }

  if (agent.capturable() && is_near(coord, agent.coord, 10)) {
    if (_visual_confusion) {
      uint32_t num_preys = 0;

      for (auto const visual_stimuli : _retina->cells_preys) {
        if (visual_stimuli != 0) {
          ++num_preys;
        }
      }
      auto tirage = d_cap(gen);
      if (!(num_preys > 0 && (tirage <= (100 / num_preys)))) {
        return false;
      }
    }
    handling_time = 10;
    return true;
  }

  return false;
}

bool Predator::capturable() const {
  return false;
}

void Predator::is_seen(Retina& retina, size_t cell_index) const {
  retina.cells_predators[cell_index] = 1;
}

Color Predator::color() const {
  return {255, 0, 0, 255};
}
