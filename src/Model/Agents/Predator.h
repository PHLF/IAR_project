#ifndef PREDATOR_H
#define PREDATOR_H

#include <cstdint>

#include "Agent.h"
#include "View/SDLWrappers.h"
#include "Model/Agents/Retina.h"
#include "Model/Evo/MarkovBrain.h"

namespace sim {
class Predator : public Agent {
 public:
  Predator(MarkovBrain const& brain_,
           uint32_t speed,
           uint32_t turn_speed,
           uint32_t segments,
           float los,
           uint32_t fov,
           bool confusion = true,
           SDL_Texture* sprite_ = nullptr);
  ~Predator();

  bool captures(Agent const& agent) override;
  bool capturable() const override;
  void is_seen(Retina& retina, size_t cell_index) const override;

  Color color() const override;

 private:
  bool _visual_confusion;
  uint32_t handling_time = 0;
};
}  // namespace sim

#endif  // PREDATOR_H
