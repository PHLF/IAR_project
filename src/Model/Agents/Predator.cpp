#include "Predator.h"
#include "Model/Agents/Agent.h"
#include <pcg_random.hpp>
#include <random>

using namespace sim;

auto Predator::try_captures(ffloat distance, pcg32_fast& prng) -> bool
{
    std::uniform_int_distribution<uint8_t> d_cap(0, 100);

    constexpr auto HANDLING_TIME    = 10U;
    constexpr auto CAPTURE_DISTANCE = 20U;
    if (distance > CAPTURE_DISTANCE || elapsed_handling++ < HANDLING_TIME)
    {
        return false;
    }
    elapsed_handling = 0;

    if (!confusion)
    {
        return true;
    }

    uint16_t const nb_stimuli = (((get_state() << Agent::__nb_actions) >> Agent::__nb_actions) >>
                                 (MAX_STATE_SIZE - get_nb_retina_cells()))
                                    .count() |
                                1U;

    return d_cap(prng) > 100 / nb_stimuli;
}

void Predator::set(bool confusion)
{
    this->confusion = confusion;
}
