#include "Predator.h"

using namespace sim;

Predator::Predator(uint32_t speed,
                   uint32_t turn_speed,
                   uint32_t segments,
                   float los,
                   float fov,
                   bool confusion)
    : Agent(true, speed, turn_speed, segments, los, fov),
      _visual_confusion(confusion) {}

Predator::~Predator() {}

bool Predator::captures(const Agent& prey) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint8_t> d_cap(0, 100);

  bool cap = false;

  uint32_t num_preys = 0;

  if (is_near(coord, prey.coord, 5)) {
    if (_visual_confusion) {
      for (auto const visual_stimuli : _retina.cells_preys) {
        if (visual_stimuli != 0) {
          ++num_preys;
        }
      }
      auto tirage = d_cap(gen);
      if (num_preys > 0 && (tirage <= (100 / num_preys))) {
        cap = true;
        handling_time = 10;
      }
    } else {
      cap = true;
      handling_time = 10;
    }
  }
  return cap;
}
