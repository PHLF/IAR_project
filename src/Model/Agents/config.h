#ifndef AGENTS_CONFIG_H
#define AGENTS_CONFIG_H

#include "types.h"

#include <cstdint>
#include <filesystem>
#include <vector>

#include "toml++/toml.h"

namespace config
{
struct Attributes
{

    uint8_t      speed           : 4 {};
    uint8_t      rate_of_turn    : 4 {};
    uint8_t      nb_retina_cells : 4 {};
    uint8_t      nb_memory_cells : 4 {};
    uint8_t      fov{};
    uint8_t      los{};
    sim::ETypeId targets{sim::__none};
};
struct MarkovBrain
{
    bool                  evolve{};
    int                   ancestor_genes{};
    std::filesystem::path file_to_load{};
    int                   max_inputs{};
    int                   max_outputs{};
};

struct Simulation
{
    int population_size{};
};

struct Prey
{
    Attributes  attributes;
    MarkovBrain markov_brain;
    Simulation  simulation;
};
struct Predator : public Prey
{
    bool confusion{};
};

auto parse_config(toml::table& settings)->std::vector<std::variant<Prey, Predator>>;


} // namespace config

#endif // AGENTS_CONFIG_H