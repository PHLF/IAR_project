#include "ProbaLogicGate.h"

using namespace sim;

ProbabilisticLogicGate::ProbabilisticLogicGate(
    std::vector<uint32_t>&& input_nodes_ids,
    std::vector<uint32_t>&& output_nodes_ids,
    std::vector<uint8_t>&& table)
    : _input_nodes_ids(input_nodes_ids),
      _output_nodes_ids(output_nodes_ids),
      _table(table) {}
