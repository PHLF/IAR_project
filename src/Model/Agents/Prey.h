#ifndef PREY_H
#define PREY_H

#include <cstdint>
#include <vector>

#include "Agent.h"
#include "Model/Evo/MarkovBrain.h"
#include "Retina.h"
#include "View/SDLWrappers.h"

namespace sim {
class Prey : public Agent {
 public:
  Prey(MarkovBrain const& brain_,
       uint32_t speed,
       uint32_t turn_speed,
       uint32_t segments,
       float los,
       float fov,
       SDL_Texture* sprite_);
  ~Prey();

  std::vector<uint8_t> get_state() override;
  void is_seen(Retina& retina, size_t cell_index) const override;
  Color color() const override;
};
}  // namespace sim

#endif  // PREY_H
