#include "Predator.h"

#include "Prey.h"
#include "Retina.h"
#include "pcg_random.hpp"

using namespace sim;

Predator::Predator(const MarkovBrain& brain_,
                   size_t nb_memory_cells,
                   uint32_t speed_,
                   uint32_t turn_speed_,
                   uint32_t segments,
                   uint32_t los,
                   uint32_t fov,
                   bool confusion,
                   SDL_Texture* sprite_)
    : Agent(brain_, nb_memory_cells, speed_, turn_speed_, sprite_),
      _visual_confusion(confusion) {
  _retina.reset(new Retina<Prey>(segments, los, fov, coords));
  setup_sensor_ios();
}

Predator::~Predator() {}

void Predator::visit(Prey& prey) {
  thread_local pcg_extras::seed_seq_from<std::random_device> seed_source;
  thread_local pcg32_fast rng(seed_source);

  thread_local std::uniform_int_distribution<uint8_t> d_cap(0, 100);

  if (is_near(coords, prey.coords, 4)) {
    if (_visual_confusion) {
      uint32_t nb_stimuli = 0;

      const auto [layers, nb_layers] = _retina->layers();
      for (size_t i = 0; i < nb_layers; ++i) {
        nb_stimuli += layers[i]->nb_stimuli();
      }

      auto tirage = d_cap(rng);
      if (tirage > 100 / (nb_stimuli > 0 ? nb_stimuli : 1)) {
        return;
      }
    }
    handling_time = 10;
    prey.set_alive(false);
  }
}

void Predator::captures() {
  if (handling_time > 0) {
    --handling_time;
    return;
  }

  const auto [layers, nb_layers] = _retina->layers();

  for (size_t i = 0; i < nb_layers; ++i) {
    for (auto cell : layers[i]->cells()) {
      if (cell.target != nullptr) {
        cell.target->accept(*this);
      }
    }
  }
}

void Predator::accept(AgentVisitor& visitor) {
  visitor.visit(*this);
}

Color Predator::color() const {
  return {255, 0, 0, 255};
}
