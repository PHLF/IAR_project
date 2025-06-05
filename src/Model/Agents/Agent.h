#ifndef AGENT_H
#define AGENT_H

#include <bit>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <iostream>

#include "Misc/Utils.h"
#include "Model/Agents/config.h"
#include "Model/Environment/Environment.h"
#include "fmt/base.h"

namespace sim
{

template <typename T> class Agent
{
  public:
    static constexpr uint8_t MAX_STATE_SIZE{64};

    enum EActions : uint8_t
    {
        idx_turn,
        idx_turn_direction,
        idx_forward,
        __nb_actions
    };

    auto get_mut_state() -> std::bitset<MAX_STATE_SIZE>& { return state; }
    auto get_state() const -> std::bitset<MAX_STATE_SIZE> const& { return state; }

    void set_coords(Coords coords) { this->coords = coords; }
    auto get_coords() const -> Coords const& { return coords; }

    void set_orientation(uint16_t orientation) { this->orientation = orientation; }
    auto get_orientation() const -> uint16_t { return orientation; }

    void reset_state()
    {
        for (int i = 0;
             i < __nb_actions + config.state.nb_retina_cells * config.state.nb_retina_layers; ++i)
        {
            state[i] = false;
        }
    }

    void turn()
    {
        auto const new_orientation = state[idx_turn_direction]
                                       ? orientation + config.motion.rate_of_turn
                                       : orientation - config.motion.rate_of_turn;

        orientation = 360 + new_orientation;
    }

    void forward()
    {
        coords.x += config.motion.speed * cos(orientation);
        coords.y += config.motion.speed * sin(orientation);
    }

    void move(Environment const& environment)
    {
        if (state[idx_turn])
        {
            turn();
        }
        if (state[idx_forward])
        {
            forward();
        }
        environment.alter(coords);
    }

    template <int layer_idx = 0, typename K> auto observe(Agent<K> const& other) -> ffloat
    {
        auto const los = config.view.los;

        auto const distance = dist(coords, other.get_coords());
        if (distance > los * los)
        {
            return distance;
        }

        auto const fov      = config.view.fov;
        auto const cell_fov = fov / get_nb_retina_cells();

        auto const other_x = other.get_coords().x;
        auto const other_y = other.get_coords().y;

        auto const dist_x_v = other_x - coords.x;
        auto const dist_y_v = other_y - coords.y;

        const auto check_clockwise = [&, this](int i) {
            auto theta = orientation - fov / 2 + i * cell_fov;

            auto const self_x = los * cos(theta);
            auto const self_y = los * sin(theta);

            auto const normal_v_x = -self_y;
            auto const normal_v_y = self_x;

            return (dist_x_v * normal_v_x + dist_y_v * normal_v_y) > 0;
        };

        uint32_t count{0};
        for (int i = 0; i < get_nb_retina_cells(); ++i)
        {
            count |= (check_clockwise(i) & 1U) << i;
        }

        int const tgt_cell_idx = std::countr_one(count) - 1;
        if (tgt_cell_idx != -1)
        {
            state[__nb_actions + layer_idx * get_nb_retina_cells() + tgt_cell_idx] = true;
        }

        return distance;
    }

    auto cell_bounds(int idx) -> std::array<ffloat, 4>
    {
        auto const los      = config.view.los;
        auto const fov      = config.view.fov;
        auto const cell_fov = fov / get_nb_retina_cells();
        auto const theta    = orientation - fov / 2 + idx * cell_fov;

        auto const left_bound_top_x = coords.x + los * cos(theta);
        auto const left_bound_top_y = coords.y + los * sin(theta);

        // clang-format off
        return {coords.x,          coords.y,
                left_bound_top_x,  left_bound_top_y};
        // clang-format on
    }

    static auto get_nb_retina_cells() -> uint8_t
    {
        return config.state.nb_retina_cells / config.state.nb_retina_layers;
    }

    template <int layer_idx = 0> auto has_stimuli(int idx) -> bool
    {
        return state[__nb_actions + layer_idx * get_nb_retina_cells() + idx];
    }

    static void set(Config config)
    {
        if (config.state.nb_memory_cells + config.state.nb_retina_cells > Agent::MAX_STATE_SIZE)
        {
            fmt::println(stderr,
                         "The state size for an agent (number of memory cells + "
                         "total number of retina cells) cannot exceed: {}",
                         Agent::MAX_STATE_SIZE);

            std::terminate();
        }
        Agent::config = config;
    }

  private:
    static Config config;

    std::bitset<MAX_STATE_SIZE> state{};
    Coords                      coords{};
    uint16_t                    orientation{};

    template <typename K>
    friend std::ostream& operator<<(std::ostream& stream, const Agent<K>& a);
};

template <typename T> Config Agent<T>::config{};

template <typename T> std::ostream& operator<<(std::ostream& os, const Agent<T>& a)
{
    os << " speed : " << Agent<T>::config.motion.speed;
    os << " turn speed :  " << Agent<T>::config.motion.rate_of_turn;
    os << " orientation : " << a.orientation;
    os << " coordinates : " << a.coords.x << " " << a.coords.y;

    return os;
}

} // namespace sim

#endif // AGENT_H
