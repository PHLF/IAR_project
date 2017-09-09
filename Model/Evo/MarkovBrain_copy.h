#ifndef MARKOVBRAIN2_H
#define MARKOVBRAIN2_H

#include "../../Misc/Globals.h"

namespace sim {
class MarkovBrain {
 public:
  MarkovBrain(uint32_t nb_inputs, uint32_t nb_outputs, uint32_t nb_genes);
  ~MarkovBrain();
  friend std::ostream& operator<<(std::ostream& os,
                                  MarkovBrain const& markov_brain);
  friend std::istream& operator>>(std::istream& is,
                                  MarkovBrain const& markov_brain);

 private:
  uint32_t _nb_inputs;
  uint32_t _nb_outputs;
  uint32_t _nb_genes;
  std::vector<uint8_t> _genome;
  std::vector<std::vector<uint8_t>> _prob_logic_gates;
};
}
#endif  // MARKOVBRAIN2_H
