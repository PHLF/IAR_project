#include <Modele/Agents/Agent.h>

using namespace sim;

Agent::Agent(bool predates_,
             uint32_t speed_,
             uint32_t turn_speed_,
             uint32_t orientation_,
             uint32_t segments_,
             float viewDepth,
             float fov)
    : predates(predates_),
      orientation(orientation_),
      speed(speed_),
      turn_speed(turn_speed_),
      turned_left(false),
      turned_right(false),
      handling_time(0),
      _retina(new Retina(segments_, viewDepth, fov)) {}

Agent::~Agent() {}

void Agent::turnLeft() {
  orientation = (360 + orientation + turn_speed) % 360;
  turned_left = true;
}

void Agent::turnRight() {
  orientation = (360 + orientation - turn_speed) % 360;
  turned_right = true;
}

std::vector<uint8_t> Agent::get_input() {
  std::vector<uint8_t> input;
  for (auto const cell_prey : _retina->cells_preys) {
    input.push_back(cell_prey);
  }
  if (!predates) {
    for (auto const cell_pred : _retina->cells_predators) {
      input.push_back(cell_pred);
    }
  }
  input.push_back(turned_left ? 1 : 0);
  input.push_back(turned_right ? 1 : 0);

  return input;
}

std::ostream& sim::operator<<(std::ostream& os, const Agent& a) {
  os << "Speed : " << a.speed;
  os << " turn speed :  " << a.turn_speed;
  os << " orientation : " << a.orientation;
  os << " Coordinates : " << a.coord.x << " " << a.coord.y;
  return os;
}
