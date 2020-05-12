#ifndef PREY_H
#define PREY_H

#include <cstdint>
#include <vector>

#include "Agent.h"
#include "Model/Evo/MarkovBrain.h"
#include "View/SDLWrappers.h"

namespace sim {
class Prey : public Agent {
 public:
  Prey(MarkovBrain const& brain_,
       size_t nb_memory_cells,
       uint32_t speed_,
       uint32_t turn_speed_,
       uint32_t segments,
       uint32_t los,
       uint32_t fov,
       SDL_Texture* sprite_);
  ~Prey() override;

  std::vector<uint8_t> get_state() override;
  void accept(AgentVisitor& visitor) override;
  Color color() const override;
};
}  // namespace sim

#endif  // PREY_H
