#ifndef PREDATOR_H
#define PREDATOR_H

#include <cstdint>
#include <pcg_random.hpp>

#include "Model/Agents/Agent.h"
#include "Model/Agents/config.h"

namespace sim
{
class Predator : public Agent<Predator>
{
  public:
    auto try_captures(ffloat distance, pcg32_fast& prng) -> bool;

    using Agent<Predator>::set;
    void set(bool confusion);

  private:
    uint16_t elapsed_handling{0};
    bool     confusion{false};
};
} // namespace sim

#endif // PREDATOR_H
