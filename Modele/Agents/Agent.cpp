#include <Modele/Agents/Agent.h>

using namespace sim;

double deg_to_rad(int32_t orientation) {
  return ((M_PI * orientation) / 180);
}

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

void Agent::moveForward() {
  _coord.x += _speed * cos(deg_to_rad(_orientation));
  _coord.y += _speed * sin(deg_to_rad(_orientation));
}

void Agent::turnLeft() {
  _orientation -= _turn_speed;
}

void Agent::turnRight() {
  _orientation += _turn_speed;
}

std::ostream& sim::operator<<(std::ostream& os, const Agent& a) {
  os << "Speed : " << a._speed;
  os << " turn speed :  " << a._turn_speed;
  os << " orientation : " << a._orientation;
  os << " Coordinates : " << a._coord.x << " " << a._coord.y;
  return os;
}
