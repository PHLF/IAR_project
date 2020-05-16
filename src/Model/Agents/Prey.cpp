#include "Prey.h"

#include <iterator>

#include "Predator.h"
#include "Retina.h"

using namespace sim;

Prey::Prey(const MarkovBrain& brain_,
           size_t nb_memory_cells,
           uint32_t speed_,
           uint32_t turn_speed_,
           uint32_t segments,
           uint32_t los,
           uint32_t fov,
           SDL_Texture* sprite_)
    : Agent(brain_, nb_memory_cells, speed_, turn_speed_, sprite_) {
  _retina.reset(new Retina<Prey, Predator>(segments, los, fov, coords));
  setup_sensor_ios();
}

Prey::~Prey() {}

void Prey::accept(AgentVisitor& visitor) {
  visitor.visit(*this);
}

Color Prey::color() const {
  return {0, 0, 0, 255};
}
