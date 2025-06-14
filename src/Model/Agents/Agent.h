#ifndef AGENT_H
#define AGENT_H

#include <bit>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <iostream>

#include "Misc/Utils.h"
#include "Model/Agents/config.h"
#include "Model/Agents/types.h"
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
        auto const nb_reactive_state =
            __nb_actions + attributes.nb_retina_cells * get_nb_retina_layers();
        state &= ~(1 << std::bit_width(nb_reactive_state)) - 1;
    }

    void turn()
    {
        auto const new_orientation = state[idx_turn_direction]
                                       ? orientation + attributes.rate_of_turn
                                       : orientation - attributes.rate_of_turn + 360;
    }

    void forward()
    {
        coords.x += attributes.speed * cos(orientation);
        coords.y += attributes.speed * sin(orientation);
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

    template <typename K> auto observe(Agent<K> const& other) -> ffloat
    {
        auto const distance = dist(coords, other.get_coords());
        if ((TypeId<K>::type_idx & attributes.targets) == 0 ||
            distance > attributes.los * attributes.los)
        {
            return distance;
        }

        auto const cell_fov = attributes.fov / get_nb_retina_cells();

        auto const other_x = other.get_coords().x;
        auto const other_y = other.get_coords().y;

        auto const dist_x_v = other_x - coords.x;
        auto const dist_y_v = other_y - coords.y;

        const auto check_clockwise = [this](int i) {
            auto theta = orientation - attributes.fov / 2 + i * cell_fov;

            auto const self_x = attributes.los * cos(theta);
            auto const self_y = attributes.los * sin(theta);

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
            state[get_retina_idx<K>(tgt_cell_idx)] = true;
        }

        return distance;
    }

    auto cell_bounds(int idx) -> std::array<ffloat, 4>
    {
        auto const cell_fov = attributes.fov / get_nb_retina_cells();
        auto const theta    = orientation - attributes.fov / 2 + idx * cell_fov;

        auto const left_bound_top_x = coords.x + attributes.los * cos(theta);
        auto const left_bound_top_y = coords.y + attributes.los * sin(theta);

        return {coords.x, coords.y, left_bound_top_x, left_bound_top_y};
    }

    auto get_nb_retina_cells() -> uint8_t { return attributes.nb_retina_cells; }

    template <typename K> auto has_stimuli(int idx) -> bool
    {
        return state[get_retina_idx<K>(idx)];
    }

    void set(config::Attributes const& attributes)
    {
        if (auto const state_sz =
                attributes.nb_memory_cells + attributes.nb_retina_cells * get_nb_retina_layers();
            state_sz > Agent::MAX_STATE_SIZE)
        {
            fmt::println(stderr,
                         "The state size for the agent (number of memory cells + "
                         "total number of retina cells) is {} or it cannot exceeds {}",
                         state_sz, Agent::MAX_STATE_SIZE);

            std::terminate();
        }
        this->attributes = attributes;
    }

    template <typename K> constexpr auto get_retina_idx(int cell_idx) -> int
    {
        auto const target_idx = attributes.targets >> (std::countr_zero(TypeId<K>::type_idx) + 1);
        return __nb_actions + std::popcount(target_idx) * get_nb_retina_cells() + cell_idx;
    }

    constexpr auto get_nb_retina_layers() -> int
    {
        return std::popcount(static_cast<uint8_t>(attributes.targets));
    }

  private:
    std::bitset<MAX_STATE_SIZE> state{};
    Coords                      coords{};
    uint16_t                    orientation{};
    config::Attributes          attributes{};

    template <typename K> friend std::ostream& operator<<(std::ostream& stream, const Agent<K>& a);
};

template <typename T> std::ostream& operator<<(std::ostream& os, const Agent<T>& a)
{
    os << " speed : " << a.attributes.speed;
    os << " turn speed :  " << a.attributes.rate_of_turn;
    os << " orientation : " << a.orientation;
    os << " coordinates : " << a.coords.x << " " << a.coords.y;

    return os;
}

} // namespace sim

#endif // AGENT_H
