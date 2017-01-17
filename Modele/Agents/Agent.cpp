#include <Modele/Agents/Agent.h>

using namespace sim;

Agent::Agent(bool predates_,
             uint32_t speed_,
             uint32_t turn_speed_,
             uint32_t orientation_,
             uint32_t segments_,
             float view_depth_,
             float fov_)
    : predates(predates_),
      orientation(orientation_),
      speed(speed_),
      turn_speed(turn_speed_),
      _retina(new Retina(segments_, view_depth_, fov_)) {}

Agent::~Agent() {}

void Agent::turnLeft() {
  orientation -= turn_speed;
}

void Agent::turnRight() {
  orientation += turn_speed;
}

std::ostream& sim::operator<<(std::ostream& os, const Agent& a) {
  os << "Speed : " << a.speed;
  os << " turn speed :  " << a.turn_speed;
  os << " orientation : " << a.orientation;
  os << " Coordinates : " << a.coord.x << " " << a.coord.y;
  return os;
}
