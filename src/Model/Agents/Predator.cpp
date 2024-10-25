#include "Predator.h"
#include "Model/Agents/Agent.h"

using namespace sim;

namespace
{
Predator::HandlingTime _handling_time{10};
bool                   _visual_confusion{false};
} // namespace

auto Predator::try_captures(ffloat distance) -> bool
{
    thread_local pcg_extras::seed_seq_from<std::random_device> seed_source;
    thread_local pcg32_fast                                    rng(seed_source);

    std::uniform_int_distribution<uint8_t> d_cap(0, 100);

    constexpr auto CAPTURE_DISTANCE = 4U;
    if (distance > CAPTURE_DISTANCE || elapsed_handling++ < _handling_time.val)
    {
        return false;
    }
    elapsed_handling = 0;

    if (!_visual_confusion)
    {
        return true;
    }

    uint16_t const nb_stimuli = (((get_state() << Agent::__nb_actions) >> Agent::__nb_actions) >>
                                 (MAX_STATE_SIZE - get_nb_retina_cells()))
                                    .count() |
                                1U;

    return d_cap(rng) > 100 / nb_stimuli;
}

void Predator::set(bool confusion)
{
    _visual_confusion = confusion;
}

void Predator::set(HandlingTime handling_time)
{
    _handling_time = handling_time;
}