#ifndef MN_H
#define MN_H

#include <Divers/Utils.h>

namespace sim {
class Mn {
  using Matrix2D = std::vector<std::vector<uint8_t>>;

 public:
  Mn(uint32_t nbAct, uint32_t nbSens);
  ~Mn();

  void print_tirages();
  std::vector<uint8_t> choose_action(std::vector<uint8_t> input);

  void save_as_file(std::string id);
  void load_file(std::string id);
  void gaussian_random_mutation(uint8_t alpha);

  Matrix2D const& markov_brain() const;

 private:
  uint32_t _nb_actions;
  uint32_t _nb_sensors;
  uint32_t _nb_states;

  Matrix2D _markov_brain;

  void _random_fill();
};
}
#endif  // MN_H
