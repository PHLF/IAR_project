#ifndef PREDATOR_H
#define PREDATOR_H

#include <cstdint>

#include "Model/Agents/Agent.h"

namespace sim
{
class Predator : public Agent<Predator>
{
  public:
    struct HandlingTime
    {
        uint16_t val{};
    };

    auto try_captures(ffloat distance) -> bool;

    using Agent<Predator>::set;
    static void set(bool confusion);
    static void set(HandlingTime handling_time);

  private:
    uint16_t elapsed_handling{0};
};
} // namespace sim

#endif // PREDATOR_H
