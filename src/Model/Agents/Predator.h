#ifndef PREDATOR_H
#define PREDATOR_H

#include <cstdint>

#include "Agent.h"
#include "Model/Evo/MarkovBrain.h"
#include "View/SDLWrappers.h"

class AgentVisitor;

namespace sim {
class Predator : public Agent, AgentVisitor {
 public:
  Predator(MarkovBrain const& brain_,
           size_t nb_memory_cells,
           uint32_t _speed,
           uint32_t _turn_speed,
           uint32_t segments,
           uint32_t los,
           uint32_t fov,
           bool confusion = true,
           SDL_Texture* sprite_ = nullptr);
  ~Predator() override;

  void captures() override;
  void accept(AgentVisitor& visitor) override;
  void visit(Prey& prey) override;

  Color color() const override;

 private:
  bool _visual_confusion;
  uint32_t handling_time = 0;
};
}  // namespace sim

#endif  // PREDATOR_H
