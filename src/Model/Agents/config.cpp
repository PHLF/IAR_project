#include "config.h"
#include <ranges>

namespace config
{

auto parse_config(toml::table& settings) -> std::vector<std::variant<Prey, Predator>>
{
    std::vector<std::variant<Prey, Predator>> agent_configs;

    for (auto const& agent_config : *settings["agents"].as_array() |
             std::views::transform([](auto&& node) { return node.as_table(); }))
    {
        Attributes attributes{};
        attributes.nb_memory_cells = (*agent_config)["memory cells"].ref<int64_t>();
        attributes.speed           = (*agent_config)["speed"].ref<int64_t>();
        attributes.rate_of_turn    = (*agent_config)["turn rate"].ref<int64_t>();
        attributes.fov             = (*agent_config)["retina"]["field of view"].ref<int64_t>();
        attributes.los             = (*agent_config)["retina"]["line of sight"].ref<int64_t>();
        attributes.nb_retina_cells = (*agent_config)["retina"]["cells per layer"].ref<int64_t>();
        for (auto const& tgt : *((*agent_config)["retina"]["targets"].as_array()))
        {
            attributes.targets = static_cast<sim::ETypeId>(attributes.targets |
                  (tgt.ref<std::string>() == "prey"     ? sim::TypeId<sim::Prey>::type_idx
                 : tgt.ref<std::string>() == "predator" ? sim::TypeId<sim::Predator>::type_idx
                                                        : sim::TypeId<>::type_idx));
        }
        MarkovBrain markov_brain_conf{};
        markov_brain_conf.evolve = (*agent_config)["markov brain"]["evolve"].ref<bool>();
        markov_brain_conf.ancestor_genes = (*agent_config)["markov brain"]["ancestor genes"].ref<int64_t>();
        markov_brain_conf.file_to_load = (*agent_config)["markov brain"]["file to load"].ref<std::string>();
        markov_brain_conf.max_inputs = (*agent_config)["markov brain"]["max inputs"].ref<int64_t>();
        markov_brain_conf.max_outputs = (*agent_config)["markov brain"]["max outputs"].ref<int64_t>();

        Simulation simulation_conf{};
        simulation_conf.population_size = markov_brain_conf.max_outputs =
            (*agent_config)["simulation"]["population size"].ref<int64_t>();

        Prey prey_conf{attributes, markov_brain_conf, simulation_conf};

        auto const& agent_type = (*agent_config)["type"].ref<std::string>();
        if (agent_type == "prey")
        {
            agent_configs.emplace_back(prey_conf);
        }
        else if (agent_type == "predator")
        {
            agent_configs.emplace_back(Predator{prey_conf, (*agent_config)["confusion"].ref<bool>()});
        }
    }

    return agent_configs;
}
} // namespace config