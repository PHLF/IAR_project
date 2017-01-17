#include <Modele/Agents/Agent.h>

using namespace sim;

Agent::Agent(bool predates_,
             uint32_t speed,
             uint32_t turnSpeed,
             uint32_t orientation,
             uint32_t segments,
             float viewDepth,
             float fov)
    : predates(predates_), _retina(new Retina(segments, viewDepth, fov)) {
  // ctor
  _speed = speed;
  _turn_speed = turnSpeed;
  _orientation = orientation;
}

Agent::~Agent() {}

void Agent::turnLeft() {
  _orientation -= _turn_speed;
}

void Agent::turnRight() {
  _orientation += _turn_speed;
}

void Agent::observe() {
  _retina->compute_local_vectors(_coord, _orientation);
}

std::ostream& sim::operator<<(std::ostream& os, const Agent& a) {
  os << "Speed : " << a._speed;
  os << " turn speed :  " << a._turn_speed;
  os << " orientation : " << a._orientation;
  os << " Coordinates : " << a._coord.x << " " << a._coord.y;
  return os;
}
