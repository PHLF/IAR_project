#ifndef GLOBALS_H
#define GLOBALS_H

#include <algorithm>
#include <cstdint>
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <thread>
#include <vector>

namespace sim {

class Agent;

using Agents = std::vector<std::unique_ptr<Agent>>;
}

#endif  // GLOBALS_H
