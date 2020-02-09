#ifndef AGENTS_H
#define AGENTS_H

#include <vector>
#include <memory>

namespace sim {
class Agent;

using Agents = std::vector<std::unique_ptr<Agent>>;
}  // namespace sim

#endif  // AGENTS_H
