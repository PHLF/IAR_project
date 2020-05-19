#ifndef AGENT_H
#define AGENT_H

#include <cstdint>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "AgentVisitor.h"
#include "Io.h"
#include "Model/Environment/Environment.h"
#include "Model/Evo/MarkovBrain.h"
#include "View/SDLWrappers.h"
#include "agents.h"

namespace sim {

class RetinaBase;

class Agent {
 public:
  Agent(MarkovBrain const& brain_,
        size_t nb_memory_cells,
        uint32_t _speed,
        uint32_t _turn_speed,
        SDL_Texture* sprite_);
  virtual ~Agent();
  static size_t nb_actions();

  virtual void turn_left();
  virtual void turn_right();
  virtual void forward();
  virtual void move(Environment const& environment);
  virtual void observe(Agents const& agents);
  virtual void captures();
  virtual void accept(AgentVisitor& visitor) = 0;

  virtual Color color() const = 0;

  RetinaBase const& retina() const;

  SDL_Texture* get_sprite();

  uint32_t orientation;
  Coords coords;

  void set_alive(bool value);
  bool is_alive();

 protected:
  void setup_sensor_ios();

  std::vector<IO> _ios;

  std::vector<uint8_t> memory_cells;

  uint32_t _speed;
  uint32_t _turn_speed;

  bool _alive;
  bool _turned_left;
  bool _turned_right;
  bool _moved_forward;

  std::unique_ptr<RetinaBase> _retina;
  MarkovBrain const& _brain;
  SDL_Texture* _sprite;

  friend std::ostream& operator<<(std::ostream& stream, const Agent& a);
};
std::ostream& operator<<(std::ostream& stream, const Agent& a);

}  // namespace sim

#endif  // AGENT_H
