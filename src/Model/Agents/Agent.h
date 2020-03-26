#ifndef AGENT_H
#define AGENT_H

#include <cstdint>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "AgentVisitor.h"
#include "Model/Environment/Environment.h"
#include "Model/Evo/MarkovBrain.h"
#include "View/SDLWrappers.h"
#include "agents.h"

namespace sim {

class RetinaBase;

class Agent {
 public:
  Agent(MarkovBrain const& brain_,
        uint32_t speed,
        uint32_t turn_speed,
        SDL_Texture* sprite_);
  virtual ~Agent();

  virtual void turn_left();
  virtual void turn_right();
  virtual void forward();
  virtual void move(Environment const& environment);
  virtual void observe(Agents const& agents);
  virtual void captures();
  virtual void accept(AgentVisitor& visitor) = 0;
  virtual std::vector<uint8_t> get_state();

  virtual Color color() const = 0;

  RetinaBase const& retina() const;

  SDL_Texture* get_sprite();

  uint32_t orientation;
  uint32_t speed;
  uint32_t turn_speed;
  Coords coords;
  bool turned_left = false;
  bool turned_right = false;

 protected:
  struct IO {
    bool input;
    std::function<void(void)> output;
  };

  std::vector<IO> _ios;

  std::unique_ptr<RetinaBase> _retina;
  MarkovBrain const& _brain;
  SDL_Texture* _sprite;

  friend std::ostream& operator<<(std::ostream& stream, const Agent& a);
};
std::ostream& operator<<(std::ostream& stream, const Agent& a);

}  // namespace sim

#endif  // AGENT_H
