﻿#ifndef MARKOVBRAIN2_H
#define MARKOVBRAIN2_H

#include "../../Misc/Globals.h"
#include "ProbaLogicGate.h"

namespace sim {
class MarkovBrain {
 public:
  MarkovBrain(uint32_t max_inputs,
              uint32_t max_outputs,
              uint32_t nb_nodes,
              uint32_t nb_ancestor_genes);
  ~MarkovBrain();
  friend std::ostream& operator<<(std::ostream& os,
                                  MarkovBrain const& markov_brain);
  friend std::istream& operator>>(std::istream& is,
                                  MarkovBrain const& markov_brain);

  void init_genome();
  void init_genome(uint64_t seed);

 private:
  uint64_t _current_seed;
  uint32_t _max_inputs;
  uint32_t _max_outputs;
  uint32_t _nb_nodes;
  uint32_t _nb_ancestor_genes;
  std::vector<uint8_t> _genome;
  std::vector<ProbabilisticLogicGate> _prob_logic_gates;

  std::vector<uint8_t> _build_gene();
  void _build_from_genome();
  void _build_plg(uint32_t index, std::vector<uint32_t>& genes_start_positions);
};
}
#endif  // MARKOVBRAIN2_H
