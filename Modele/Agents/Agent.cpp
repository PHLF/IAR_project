#include <Modele/Agents/Agent.h>

Agent::Agent(uint32_t speed,
             uint32_t turnSpeed,
             uint32_t orientation,
             uint32_t segments,
             float viewDepth,
             float fov)
    : _retina(new Retina(segments, viewDepth, fov)) {
  // ctor
}

Agent::~Agent() {}

void Agent::moveForward() {}

void Agent::turnLeft() {}

void Agent::turnRight() {}

void Agent::observe() {}

bool Agent::run() {
  bool exec = false;

  return exec;
}
