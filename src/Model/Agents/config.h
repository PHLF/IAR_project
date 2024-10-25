#ifndef AGENTS_CONFIG_H
#define AGENTS_CONFIG_H

#include "Model/Evo/MarkovBrain.h"
#include "SDL_render.h"
#include "View/SDLWrappers.h"

#include <cstdint>

namespace sim
{
struct Config
{
    struct
    {
        uint8_t speed{};
        uint8_t rate_of_turn{};
    } motion;
    struct
    {
        uint8_t nb_retina_layers{1};
        uint8_t nb_retina_cells{};
        uint8_t nb_memory_cells{};
    } state;
    struct
    {
        uint8_t fov{};
        uint8_t los{};
    } view;
};
} // namespace sim

#endif // AGENTS_CONFIG_H