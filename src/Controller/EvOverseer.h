#ifndef LIGHTSIM_H
#define LIGHTSIM_H

#include <filesystem>
#include <random>
#include <unordered_map>

#include "Model/Evo/MarkovBrain.h"
#include "toml++/toml.h"

namespace sim
{
class EvOverseer
{
  public:
    EvOverseer();

    friend std::ostream& operator<<(std::ostream& os, EvOverseer const& lightsim);

    void load_settings(std::filesystem::path settings);
    void sim();

  private:
    std::mt19937_64           prng;

    static constexpr std::string_view DEFAULT_SETTINGS{R"(
["genome mutation"."per gene probability"]
deletion = 0.01
duplication = 0.005
"new gene insertion" = 0.005

["genome mutation"."per site probability"]
copy = 0.025
deletion = 0.05
"gaussian mutation" = 0.05
insertion = 0.025
substitution = 0.05


[[agents]]
type = predator
"memory cells" = 12
number = 1
speed = 1
"turn rate" = 8
confusion = false

[agents."markov brain"]
evolve = false
"ancestor genes" = 12
"file to load" = ""
"max inputs" = 4
"max outputs" = 4

[agents.retina]
"field of view" = 180
"line of sight" = 100
"cells per layer" = 12
targets = ["prey"]

[agents.simulation]
"population size" = 1

[[agents]]
type = prey
"memory cells" = 6
number = 50
speed = 1
"turn rate" = 4

[agents."markov brain"]
evolve = false
"ancestor genes" = 12
"file to load" = ""
"max inputs" = 4
"max outputs" = 4

[agents.retina]
"field of view" = 180
"line of sight" = 100
"cells per layer" = 12
targets = ["prey","predator"]

[agents.simulation]
"population size" = 50

[simulation]
generations = 10000
"pool size" = 128
threads = 1
ticks = 2000

[simulation.universe]
"closed curvature" = true
height = 512
width = 512

[viewport]
headless = false
height = 768
width = 768
)"};

    toml::table settings{toml::parse(DEFAULT_SETTINGS)};

    void _setup_sim();
    void _moran_process(std::unordered_map<uint64_t, uint32_t> const& mb_seeds_with_fit,
                        std::vector<MarkovBrain>&                     population);

    uint64_t _stochastic_acceptance(
        std::unordered_map<uint64_t, uint32_t> const& seeds_with_fitness);
};
std::ostream& operator<<(std::ostream& os, EvOverseer const& lightsim);
} // namespace sim
#endif // LIGHTSIM_H
