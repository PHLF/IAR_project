#include "ProbaLogicGate.h"

using namespace sim;

ProbabilisticLogicGate::ProbabilisticLogicGate(
    std::vector<uint32_t>&& input_nodes_ids,
    std::vector<uint32_t>&& output_nodes_ids,
    std::vector<uint8_t>&& table)
    : _input_nodes_ids(std::move(input_nodes_ids)),
      _output_nodes_ids(std::move(output_nodes_ids)),
      _table(std::move(table)) {
  _nb_inputs = static_cast<uint32_t>(_input_nodes_ids.size());
  _nb_outputs = static_cast<uint32_t>(_output_nodes_ids.size());
}

ProbabilisticLogicGate::ProbabilisticLogicGate(
    ProbabilisticLogicGate const& plg) {
  _input_nodes_ids = plg._input_nodes_ids;
  _output_nodes_ids = plg._output_nodes_ids;
  _nb_inputs = plg._nb_inputs;
  _nb_outputs = plg._nb_outputs;
  _table = plg._table;
}

std::vector<uint8_t> const& ProbabilisticLogicGate::table() const {
  return _table;
}

uint32_t ProbabilisticLogicGate::nb_inputs() const {
  return _nb_inputs;
}

uint32_t ProbabilisticLogicGate::nb_outputs() const {
  return _nb_outputs;
}

std::vector<uint32_t> const& ProbabilisticLogicGate::input_nodes_ids() const {
  return _input_nodes_ids;
}

std::vector<uint32_t> const& ProbabilisticLogicGate::output_nodes_ids() const {
  return _output_nodes_ids;
}
