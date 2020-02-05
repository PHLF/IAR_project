#ifndef AGENT_H
#define AGENT_H

#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

#include "Model/Environment/Environment.h"
#include "Model/Evo/MarkovBrain.h"
#include "Retina.h"
#include "View/SDLWrappers.h"

namespace sim {
class Agent;

using Agents = std::vector<std::unique_ptr<Agent>>;

class Agent {
 public:
  int32_t orientation;
  uint32_t speed;
  uint32_t turn_speed;
  Coords coord;
  bool turned_left = false;
  bool turned_right = false;

  Agent(MarkovBrain const& brain_,
        uint32_t speed,
        uint32_t turn_speed,
        uint32_t segments,
        float los,
        float fov,
        SDL_Texture* sprite_);
  virtual ~Agent();

  virtual void turn_left();
  virtual void turn_right();
  virtual void move(Environment const& environment);
  virtual void observe(Agents const& agents);
  virtual bool captures(const Agent& agent);
  virtual bool capturable() const;
  virtual void is_seen(Retina& retina, size_t cell_index) const = 0;
  virtual std::vector<uint8_t> get_state();

  virtual Color color() const = 0;

  Retina const& retina() const;

  SDL_Texture* get_sprite();

 protected:
  Retina _retina;
  MarkovBrain const& _brain;
  SDL_Texture* sprite;

  friend std::ostream& operator<<(std::ostream& stream, const Agent& a);
};
std::ostream& operator<<(std::ostream& stream, const Agent& a);

}  // namespace sim

#endif  // AGENT_H
