#ifndef LOCALTHREADSIM_H
#define LOCALTHREADSIM_H

#include <cstdint>
#include <functional>
#include <pcg_random.hpp>
#include <variant>
#include <vector>

#include "Model/Agents/Predator.h"
#include "Model/Agents/Prey.h"
#include "Model/Environment/box.h"
#include "Model/Environment/torus.h"
#include "View/MainView.h"

namespace sim
{
class Sim
{
  public:
    bool run();

    uint32_t eval_pred();
    uint32_t eval_prey();

    void set_view(MainView& view);

  private:
    uint32_t _ticks_per_run;
    uint32_t _nb_predators;
    uint32_t _nb_preys;

    std::vector<uint32_t> _preys_alive;
    pcg32_fast            prng;

    std::variant<Box, Torus> env;
    std::reference_wrapper<MainView>  _view;

    using Captured = std::monostate;
    using Agents   = std::variant<Predator, Prey, Captured>;
    std::vector<Agents> _agents;

    void _reset_sim();

    void _setup_agents();
    void _print_agents();

    void _sim_loop(uint32_t tick);

    bool _run_headless();
    bool _run_ui();
};
} // namespace sim
#endif // LOCALTHREADSIM_H
