#ifndef MN_H
#define MN_H

#include <Misc/Utils.h>

namespace sim {
class MarkovBrain {
  using Matrix2D = std::vector<std::vector<uint8_t>>;

 public:
  MarkovBrain();
  MarkovBrain(uint32_t nbAct, uint32_t nbSens);
  ~MarkovBrain();

  void set_dim(uint32_t nbAct, uint32_t nbSens);
  void random_fill(uint64_t seed);
  void print_tirages();
  std::vector<uint8_t> choose_action(std::vector<uint8_t> input);

  void save_as_file(std::string id);
  void load_file(std::string id);
  void gaussian_random_mutation(float alpha);

  Matrix2D const& markov_brain() const;

 private:
  uint32_t _nb_actions;
  uint32_t _nb_sensors;
  uint32_t _nb_states;

  Matrix2D _markov_brain;
};
}
#endif  // MN_H
