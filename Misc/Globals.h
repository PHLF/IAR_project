﻿#ifndef GLOBALS_H
#define GLOBALS_H

#include <algorithm>
#include <cstdint>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <functional>
#include <fstream>
#include <future>
#include <map>
#include <unordered_map>
#include <memory>
#include <random>
#include <thread>
#include <vector>
#include <string>
#include <sstream>

namespace sim {

class Agent;

using Agents = std::vector<std::unique_ptr<Agent>>;
}

#endif  // GLOBALS_H
