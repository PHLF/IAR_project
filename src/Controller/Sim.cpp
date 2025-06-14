#include "Sim.h"

#include "Misc/Utils.h"
#include "View/SDLWrappers.h"
#include "toml++/toml.h"

#include <chrono>
#include <cstdint>
#include <pcg_extras.hpp>
#include <random>
#include <thread>
#include <utility>
#include <variant>

#include "Model/Agents/Predator.h"
#include "Model/Agents/Prey.h"
#include "Model/Environment/box.h"
#include "Model/Environment/torus.h"
#include "View/MainView.h"

using namespace sim;

Sim::Sim(const toml::table& settings, MarkovBrain& pred_mb, MarkovBrain& prey_mb)
    : pred_mb(pred_mb), prey_mb(prey_mb), _settings(settings), _view(nullptr)
{
    const auto simulation = _settings["simulation"];

    const auto universe_width  = simulation["universe"]["width"].as_integer()->get();
    const auto universe_height = simulation["universe"]["height"].as_integer()->get();

    if (simulation["universe"]["closed curvature"].ref<bool>())
    {
        env.emplace<Torus>(universe_width, universe_height);
    }
    else
    {
        env.emplace<Box>(universe_width, universe_height);
    }

    _ticks_per_run = simulation["ticks"].as_integer()->get();
    _nb_preys      = _settings["prey"]["number"].as_integer()->get();
    _nb_predators  = _settings["predator"]["number"].as_integer()->get();
}

void Sim::_setup_agents()
{
    std::uniform_real_distribution<float>   d_x(0, env.size_x - 1);
    std::uniform_real_distribution<float>   d_y(0, env.size_y - 1);
    std::uniform_int_distribution<uint32_t> d_ori(0, 359);

    for (auto&& agent : _agents)
    {
        std::visit(overloaded{[](Captured) {},
                              [&](auto&& agent) {
                                  agent.set_coords({d_x(prng), d_y(prng)});
                                  agent.set_orientation(d_ori(prng));
                              }},
                   agent);
    }
}

void Sim::_print_agents()
{
    for (auto const& agent : _agents)
    {
        std::visit(
            overloaded{[](Captured) {}, [&](auto&& agent) { std::cout << agent << std::endl; }},
            agent);
    }
}

uint32_t Sim::eval_pred()
{
    uint32_t   fitness_predator = 0;
    const auto preys            = _nb_preys;

    for (auto const nb_prey : _preys_alive)
    {
        fitness_predator += preys - nb_prey;
    }

    return fitness_predator;
}

uint32_t Sim::eval_prey()
{
    uint32_t fitness_prey = 0;

    for (auto const nb_prey : _preys_alive)
    {
        fitness_prey += nb_prey;
    }

    return fitness_prey;
}

void Sim::set_view(MainView& view)
{
    _view = view;
}

void Sim::_reset_sim()
{
    prng.seed(pcg_extras::seed_seq_from<std::random_device>());

    _agents.clear();
    _preys_alive.clear();
    _preys_alive.resize(_ticks_per_run);

    Predator::set(pred_cfg);
    Predator::set(_settings["predator"]["confusion"].as_boolean()->get());

    for (uint32_t i = 0; i < _nb_predators; ++i)
    {
        _agents.emplace_back(Predator{});
    }

    auto prey_cfg = build_agent_cfg(_settings["prey"]);

    prey_cfg.state.nb_retina_layers = 2;

    Prey::set(prey_cfg);

    for (uint32_t i = 0; i < _nb_preys; ++i)
    {
        _agents.emplace_back(Prey{});
    }
    _setup_agents();
}

void Sim::_sim_loop(uint32_t tick)
{
    _preys_alive[tick] = tick == 0 ? _nb_preys : _preys_alive[tick - 1];

    for (auto&& agent_v : _agents)
    {
        std::visit(overloaded{[](Captured) {}, [](auto&& agent) { agent.reset_state(); }}, agent_v);
        for (auto&& other_v : _agents)
        {
            if (&agent_v == &other_v) [[unlikely]]
            {
                continue;
            }
            std::visit(overloaded{[this, tick, &other_v](Predator& agent, Prey& other) {
                                      if (agent.try_captures(agent.observe(other), prng))
                                      {
                                          other_v = Captured{};
                                          _preys_alive[tick]--;
                                      }
                                  },
                                  [](Prey& agent, Prey& other) { agent.observe(other); },
                                  [](Prey& agent, Predator& other) { agent.observe(other); },
                                  [](auto&&, auto&&) {}},
                       agent_v, other_v);
        }
        std::visit(overloaded{
                       [](Captured) {},
                       [this](Predator& agent) { pred_mb.actions(agent.get_mut_state(), prng); },
                       [this](Prey& agent) { prey_mb.actions(agent.get_mut_state(), prng); },
                   },
                   agent_v);
        std::visit(overloaded{[](Captured) {}, [this](auto&& agent) { agent.move(*_env); }},
                   agent_v);
    }

    if (!_view)
    {
        return;
    }
    _view->render_clear();

    static constexpr auto eq = [](std::array<int32_t, 2> const& coords_a, Coords const& coords_b,
                                  int32_t threshold = 3) {
        return coords_b.x <= coords_a[0] + threshold && coords_b.x >= coords_a[0] - threshold &&
               coords_b.y <= coords_a[1] + threshold && coords_b.y >= coords_a[1] - threshold;
    };

    static constexpr auto color = [](bool selected, bool stimuli_g,
                                     bool stimuli_r = false) -> Color {
        return {static_cast<uint8_t>(stimuli_r ? 255 : 0),
                static_cast<uint8_t>(stimuli_g ? 255 : 0), 0,
                static_cast<uint8_t>(selected ? 255 : 30)};
    };

    for (auto&& agent_v : _agents)
    {
        std::visit(
            overloaded{
                [](Captured) {},
                [this](Predator& agent) {
                    bool const selected = eq(_view->mouse_coords(), agent.get_coords());

                    _view->draw_sprite(static_cast<float>(agent.get_coords().x),
                                       static_cast<float>(agent.get_coords().y),
                                       agent.get_orientation(), MainView::EDataId::PREDATOR);
                    for (int i = 0; i <= agent.get_nb_retina_cells(); i++)
                    {
                        auto&& bounds = agent.cell_bounds(i);
                        _view->draw_line(
                            {static_cast<int32_t>(bounds[0]), static_cast<int32_t>(bounds[1])},
                            {static_cast<int32_t>(bounds[2]), static_cast<int32_t>(bounds[3])},
                            color(selected,
                                  agent.has_stimuli<Prey>(i) || (i > 0 && agent.has_stimuli<Prey>(i - 1))));
                    }
                },
                [this](Prey& agent) {
                    bool const selected = eq(_view->mouse_coords(), agent.get_coords());

                    _view->draw_sprite(static_cast<float>(agent.get_coords().x),
                                       static_cast<float>(agent.get_coords().y),
                                       agent.get_orientation(), MainView::EDataId::PREY);
                    for (int i = 0; i <= agent.get_nb_retina_cells(); i++)
                    {
                        auto&& bounds = agent.cell_bounds(i);
                        _view->draw_line(
                            {static_cast<int32_t>(bounds[0]), static_cast<int32_t>(bounds[1])},
                            {static_cast<int32_t>(bounds[2]), static_cast<int32_t>(bounds[3])},
                            color(selected,
                                  agent.has_stimuli<Prey>(i) || (i > 0 && agent.has_stimuli<Prey>(i - 1)),
                                  agent.has_stimuli<Predator>(i) ||
                                      (i > 0 && agent.has_stimuli<Predator>(i - 1))));
                    }
                },
            },
            agent_v);
    }
    _view->render_present();
}

bool Sim::run()
{
    _reset_sim();

    return (_view != nullptr ? _run_ui() : _run_headless());
}

bool Sim::_run_ui()
{
    using namespace std::chrono;
    using namespace std::chrono_literals;

    steady_clock::time_point start, end;

    uint32_t       tick  = 0;
    const uint32_t ticks = _ticks_per_run;

    for (tick = 0; tick < ticks; ++tick)
    {
        start = steady_clock::now();

        _view->process_events();
        if (_view->stop_requested())
        {
            return false;
        }

        _sim_loop(tick);

        end = steady_clock::now();
        std::this_thread::sleep_for(((16.67ms * 100) / _view->render_speed()) - (end - start));
    }
    return true;
}

bool Sim::_run_headless()
{
    uint32_t       tick  = 0;
    const uint32_t ticks = _ticks_per_run;

    for (tick = 0; tick < ticks; ++tick)
    {
        _sim_loop(tick);
    }
    return true;
}
