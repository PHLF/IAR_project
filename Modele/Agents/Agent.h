#ifndef AGENT_H
#define AGENT_H

#include <Divers/Globals.h>

#include <Modele/Agents/Retina.h>
#include <Modele/Environnement/utils.h>

namespace sim {
class Agent {
 public:
  const bool predates;

  Agent(bool predates_ = false,
        uint32_t speed = 1,
        uint32_t turnSpeed = 8,
        uint32_t orientation = 0,
        uint32_t segments = 7,
        float viewDepth = 100,
        float fov = 180);
  virtual ~Agent();

  virtual Coords get_coord() { return _coord; };
  virtual uint32_t get_speed() { return _speed; };
  virtual uint32_t get_turn_speed() { return _turn_speed; };
  virtual uint32_t get_orientation() { return _orientation; };
  virtual std::unique_ptr<Retina>& get_retina() { return _retina; };
  virtual void set_coord(Coords coord) { _coord = coord; };
  virtual void set_orientation(uint32_t orientation) {
    _orientation = orientation;
  };

  virtual void turnLeft();
  virtual void turnRight();
  virtual void observe();

 protected:
  uint32_t _speed;
  uint32_t _turn_speed;
  uint32_t _orientation;  // Angle in °

  Coords _coord;

  std::unique_ptr<Retina> _retina;

  friend std::ostream& operator<<(std::ostream& stream, const Agent& a);
};
}

#endif  // AGENT_H
