#ifndef LOCALTHREADSIM_H
#define LOCALTHREADSIM_H

#include <Model/Agents/Predator.h>
#include <Model/Agents/Prey.h>
#include <Model/Environment/Environment.h>
#include <Model/Evo/MarkovBrain2.h>
#include <View/MainView.h>

namespace sim {
class LocalThreadSim {
 public:
  MarkovBrain2 pred_mb;
  MarkovBrain2 prey_mb;

  LocalThreadSim(uint64_t seed,
                 std::map<std::__cxx11::string, uint32_t> &settings,
                 Environment* env,
                 MainView* view,
                 MarkovBrain2 const& pred_mb,
                 MarkovBrain2 const& prey_mb);
  bool run();

  uint32_t eval_pred();
  uint32_t eval_prey();

 private:
  std::map<std::string, std::uint32_t>& _settings;
  std::vector<uint32_t> _preys_alive;
  std::mt19937 _rd_gen;

  Environment* _env;
  MainView* _view;

  Agents _agents;

  void _reset_sim();

  void _setup_agents();
  void _print_agents();
  void _move_agents();
  void _capture_preys();
  void _observe_agents();

  void _sim_loop(uint32_t tick);

  bool _run_headless();
  bool _run_ui();
};
}
#endif  // LOCALTHREADSIM_H
