#ifndef PROBALOGICGATE_H
#define PROBALOGICGATE_H

#include <cstdint>
#include <vector>

namespace sim {
class ProbabilisticLogicGate {
 public:
  ProbabilisticLogicGate(std::vector<uint32_t>&& intput_nodes_ids,
                         std::vector<uint32_t>&& output_nodes_ids,
                         std::vector<uint8_t>&& table);

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
}  // namespace sim

#endif  // PROBALOGICGATE_H
