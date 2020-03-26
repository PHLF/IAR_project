#ifndef LOCALTHREADSIM_H
#define LOCALTHREADSIM_H

#include <cstdint>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "Model/Agents/Agent.h"
#include "Model/Environment/Environment.h"
#include "Model/Evo/MarkovBrain.h"
#include "View/MainView.h"
#include "toml++/toml.h"

namespace sim {
class Sim {
 public:
  MarkovBrain& pred_mb;
  MarkovBrain& prey_mb;

  Sim(const toml::table& settings, MarkovBrain& pred_mb, MarkovBrain& prey_mb);
  bool run();

  uint32_t eval_pred();
  uint32_t eval_prey();

  void set_view(MainView* view);

 private:
  const toml::table& _settings;

  uint32_t _ticks_per_run;
  uint32_t _nb_predators;
  uint32_t _nb_preys;

  std::vector<uint32_t> _preys_alive;
  std::mt19937 _rd_gen;

  std::unique_ptr<Environment> _env;
  MainView* _view;

  Agents _agents;

  void _reset_sim();

  void _setup_agents();
  void _print_agents();

  void _sim_loop(uint32_t tick);

  bool _run_headless();
  bool _run_ui();
};
}  // namespace sim
#endif  // LOCALTHREADSIM_H
