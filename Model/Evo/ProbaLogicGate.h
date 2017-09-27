#ifndef PROBALOGICGATE_H
#define PROBALOGICGATE_H

#include "../../Misc/Globals.h"

namespace sim {
class ProbabilisticLogicGate {
 public:
  ProbabilisticLogicGate(std::vector<uint32_t>&& intput_nodes_ids,
                         std::vector<uint32_t>&& output_nodes_ids,
                         std::vector<uint8_t>&& table);
  ProbabilisticLogicGate(ProbabilisticLogicGate const& plg);

  std::vector<uint8_t> const& table() const;
  std::vector<uint32_t> const& input_nodes_ids() const;
  std::vector<uint32_t> const& output_nodes_ids() const;
  uint32_t nb_inputs() const;
  uint32_t nb_outputs() const;

 private:
  uint32_t _nb_inputs;
  uint32_t _nb_outputs;
  std::vector<uint32_t> _input_nodes_ids;
  std::vector<uint32_t> _output_nodes_ids;
  std::vector<uint8_t> _table;
};
}

#endif  // PROBALOGICGATE_H
