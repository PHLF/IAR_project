#include "Prey.h"

using namespace sim;

Prey::Prey(uint32_t speed,
           uint32_t turn_speed,
           uint32_t segments,
           float los,
           float fov)
    : Agent(false, speed, turn_speed, segments, los, fov) {}
Prey::~Prey() {}
