#ifndef LIGHTSIM_H
#define LIGHTSIM_H

#include <filesystem>
#include <optional>

#include "Sim.h"
#include "toml++/toml.h"

namespace sim {
class EvOverseer {
 public:
  EvOverseer();
  ~EvOverseer();

  friend std::ostream& operator<<(std::ostream& os, EvOverseer const& lightsim);

  void load_settings(std::filesystem::path settings);
  void sim();

 private:
  std::vector<MarkovBrain> _pred_mb_pool;
  std::vector<MarkovBrain> _prey_mb_pool;
  std::unique_ptr<MainView> _view;

  toml::table _settings{
      {{"simulation",
        toml::table{{{"threads", 1},
                     {"ticks", 2000},
                     {"generations", 10},
                     {"pool size", 64},
                     {"evolve prey", false},
                     {"evolve predator", true},
                     {"universe", toml::table{{{"width", 512},
                                               {"height", 512},
                                               {"closed curvature", false}}}}

        }}},
       {"viewport",
        toml::table{{{"headless", false}, {"width", 768}, {"height", 768}}}},
       {"predator",
        toml::table{{{"number", 1},
                     {"speed", 1},
                     {"turn rate", 8},
                     {"confusion", false},
                     {"sight", toml::table{{
                                   {"line of sight", 100},
                                   {"field of view", 180},
                                   {"retina cells", 12},
                               }}},
                     {"markov brain", toml::table{{{"max inputs", 4},
                                                   {"max outputs", 4},
                                                   {"ancestor genes", 12},
                                                   {"file to load", ""}}}}}}},
       {"prey",
        toml::table{{{"number", 50},
                     {"speed", 1},
                     {"turn rate", 8},
                     {"sight", toml::table{{
                                   {"line of sight", 100},
                                   {"field of view", 180},
                                   {"retina cells by agent type", 12},
                               }}},
                     {"markov brain", toml::table{{{"max inputs", 4},
                                                   {"max outputs", 4},
                                                   {"ancestor genes", 12},
                                                   {"file to load", ""}}}}}}},
       {"genome mutation",
        toml::table{{{"per site probability", toml::table{{
                                                  {"copy", 0.025},
                                                  {"deletion", 0.050},
                                                  {"insertion", 0.025},
                                                  {"substitution", 0.050},
                                                  {"gaussian mutation", 0.050},
                                              }}},
                     {"per gene probability",
                      toml::table{{{"duplication", 0.005},
                                   {"deletion", 0.010},
                                   {"new gene insertion", 0.005}}}}}}}}

  };

  struct SimResult {
    std::unordered_map<uint64_t, uint32_t> pred_seeds_with_fitness;
    std::unordered_map<uint64_t, uint32_t> prey_seeds_with_fitness;
    std::string sim_output;
  };
  using OptSimResult = std::optional<SimResult>;

  void _setup_sim();
  void _moran_process(
      std::unordered_map<uint64_t, uint32_t> const& mb_seeds_with_fit,
      std::vector<MarkovBrain>& population);

  uint64_t _stochastic_acceptance(
      std::unordered_map<uint64_t, uint32_t> seeds_with_fitness);
  OptSimResult _run_thread(uint32_t thread_number,
                        uint32_t generation,
                        std::vector<MarkovBrain>& pred_pool,
                        std::vector<MarkovBrain>& prey_pool);
};
std::ostream& operator<<(std::ostream& os, EvOverseer const& lightsim);
}  // namespace sim
#endif  // LIGHTSIM_H
