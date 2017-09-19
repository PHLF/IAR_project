#ifndef PROBALOGICGATE_H
#define PROBALOGICGATE_H

#include "../../Misc/Globals.h"

namespace sim {
class ProbabilisticLogicGate {
 public:
  ProbabilisticLogicGate(std::vector<uint32_t>&& intput_nodes_ids,
                         std::vector<uint32_t>&& output_nodes_ids);

 private:
  std::vector<uint32_t> _input_nodes_ids;
  std::vector<uint32_t> _output_nodes_ids;
};
}

#endif  // PROBALOGICGATE_H
