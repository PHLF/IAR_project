#include "MarkovBrain.h"

using namespace sim;

MarkovBrain::MarkovBrain(MarkovBrain const& mb) {
  *this = mb;
}

MarkovBrain::MarkovBrain(MarkovBrain&& mb) {
  *this = mb;
}

MarkovBrain& MarkovBrain::operator=(MarkovBrain&& mb) {
  _gen = mb._gen;
  _current_seed = mb._current_seed;
  _max_inputs = mb._max_inputs;
  _max_outputs = mb._max_outputs;
  _nb_ancestor_genes = mb._nb_ancestor_genes;
  _nb_nodes = mb._nb_nodes;
  _prob_logic_gates = std::move(mb._prob_logic_gates);
  _genome = std::move(mb._genome);

  mb._max_inputs = 0;
  mb._max_outputs = 0;
  mb._nb_nodes = 0;
  mb._nb_ancestor_genes = 0;
  mb._prob_logic_gates.clear();
  mb._genome.clear();
}

MarkovBrain& MarkovBrain::operator=(MarkovBrain const& mb) {
  _gen = mb._gen;
  _current_seed = mb._current_seed;
  _max_inputs = mb._max_inputs;
  _max_outputs = mb._max_outputs;
  _nb_ancestor_genes = mb._nb_ancestor_genes;
  _nb_nodes = mb._nb_nodes;
  _prob_logic_gates = mb._prob_logic_gates;
  _genome = mb._genome;
}

MarkovBrain::MarkovBrain(uint32_t max_inputs,
                         uint32_t max_outputs,
                         uint32_t nb_nodes,
                         uint32_t nb_ancestor_genes)
    : _max_inputs(max_inputs),
      _max_outputs(max_outputs),
      _nb_nodes(nb_nodes),
      _nb_ancestor_genes(nb_ancestor_genes) {
  _init_seed();
  _generate_genome();
  _instantiate();
}

MarkovBrain::~MarkovBrain() {}

std::ostream& sim::operator<<(std::ostream& os, const MarkovBrain& mb) {
  os << "max_inputs " << mb._max_inputs << std::endl;
  os << "max_outputs " << mb._max_outputs << std::endl;
  os << "nb_nodes " << mb._nb_nodes << std::endl;

  for (auto const locus : mb._genome) {
    os << locus << ' ';
  }

  return os;
}

std::istream& sim::operator>>(std::istream& is, MarkovBrain& mb) {
  std::string locus;
  std::vector<uint8_t> tmp_genome;

  is.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
  is >> mb._max_inputs;
  is.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
  is >> mb._max_outputs;
  is.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
  is >> mb._nb_nodes;

  while (std::getline(is, locus, ' ')) {
    tmp_genome.push_back(static_cast<uint8_t>(locus.front()));
  }

  mb._genome = std::move(tmp_genome);
  mb._instantiate();
}

void MarkovBrain::_init_seed() {
  std::random_device rd;
  _current_seed = rd();
  _gen.seed(_current_seed);
}

void MarkovBrain::_instantiate() {
  uint8_t current_symbol = 0;
  uint8_t next_symbol = 0;

  for (uint32_t i = 0; i < _genome.size() - 1; ++i) {
    current_symbol = _genome[i];
    next_symbol = _genome[i + 1];

    if (current_symbol == 42 && next_symbol == 213) {
      _instantiate_plg(i);
    }
  }
}

void MarkovBrain::_instantiate_plg(uint32_t index) {
  bool done = false;
  uint8_t current_symbol;
  uint8_t next_symbol;
  uint32_t nb_inputs;
  uint32_t nb_outputs;
  std::vector<uint32_t> input_nodes_ids;
  std::vector<uint32_t> output_nodes_ids;
  std::vector<uint8_t> table(0);

  std::vector<uint8_t> const& genome = _genome;
  uint64_t const genome_length = genome.size();
  uint64_t plg_size = 0;

  auto increase_step = [&](uint32_t step) -> void {
    index = (index + step) % (genome_length - 1);

    current_symbol = genome[index];
    next_symbol = genome[index + 1];
  };

  increase_step(2);

  nb_inputs =
      static_cast<uint32_t>(std::floor(current_symbol / 255.0 * _max_inputs));
  nb_outputs =
      static_cast<uint32_t>(std::floor(next_symbol / 255.0 * _max_outputs));

  plg_size = static_cast<uint64_t>((1 << nb_inputs) * (1 << nb_outputs));

  increase_step(2);

  for (uint32_t i = 0; i < _max_inputs; ++i) {
    if (input_nodes_ids.size() < static_cast<uint64_t>(nb_inputs)) {
      input_nodes_ids.emplace_back(
          std::lround((current_symbol * _nb_nodes) / 255.0 - 0.5));
    }

    increase_step(1);
  }
  for (uint32_t i = 0; i < _max_outputs; ++i) {
    if (output_nodes_ids.size() < static_cast<uint64_t>(nb_outputs)) {
      output_nodes_ids.emplace_back(
          std::lround((current_symbol * _nb_nodes) / 255.0 - 0.5));
    }

    increase_step(1);
  }

  table = std::vector<uint8_t>(plg_size);

  for (uint32_t i = 0; i < (1 << nb_inputs); ++i) {
    uint32_t temp_index = index;
    uint32_t row_sum = 0;

    for (uint32_t j = 0; j < (1 << nb_outputs); ++j) {
      row_sum += genome[temp_index] + 1;
      temp_index = (temp_index + 1) % (genome_length - 1);
    }

    for (uint32_t j = 0; j < (1 << nb_outputs); ++j) {
      table[i * (1 << nb_outputs) + j] =
          ((current_symbol + 1) * 100) / (row_sum);
      if (i * (1 << nb_outputs) + j == plg_size - 1) {
        done = true;
        table.shrink_to_fit();
      }
      increase_step(1);
    }
  }
  _prob_logic_gates.emplace_back(
      ProbabilisticLogicGate(std::move(input_nodes_ids),
                             std::move(output_nodes_ids), std::move(table)));
}

void MarkovBrain::_generate_genome() {
  for (uint32_t i = 0; i < _nb_ancestor_genes; ++i) {
    auto gene = _build_gene();
    std::move(std::begin(gene), std::end(gene), std::back_inserter(_genome));
  }
  _genome.shrink_to_fit();
}

void MarkovBrain::generate_genome(uint64_t seed) {
  _current_seed = seed;
  _gen.seed(seed);

  _generate_genome();
}

void MarkovBrain::gaussian_mutation() {
  // Probability [1, 100] that a gene mutation actually happen. Hardcoded value
  // ATM. Lessen the risk of mutating a start codon (thus destroying PLGs).

  using param = std::normal_distribution<>::param_type;

  uint8_t const mutation_proba = 1;
  double const sigma = 1 / 6;

  uint8_t tmp = 0;

  std::uniform_int_distribution<uint8_t> proba_uni_dist{1, 100};
  std::normal_distribution<> byte_gaussian_dist{50.0, sigma};

  for (auto& gene_val : _genome) {
    if (proba_uni_dist(_gen) == mutation_proba) {
      tmp = static_cast<uint8_t>(byte_gaussian_dist(
          _gen, param{static_cast<double>(gene_val), sigma}));
      gene_val = tmp;
    }
  }
}

void MarkovBrain::gaussian_mutation(uint64_t seed) {
  _current_seed = seed;
  _gen.seed(seed);

  gaussian_mutation();
}

std::vector<uint8_t> MarkovBrain::actions(std::vector<uint8_t> states) const {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint8_t> d_uni{1, 100};

  std::vector<uint8_t> states_cp = states;

  for (auto const& plg : _prob_logic_gates) {
    uint32_t state = 0;
    for (auto const node_id : plg.input_nodes_ids()) {
      // Converts array of "booleans" to an integer
      state |= states[node_id] << 1;
    }
    state = state >> 1;

    for (uint32_t i = 0; i < plg.nb_outputs(); ++i) {
      uint8_t action_proba = plg.table()[state * plg.nb_outputs() + i];
      uint32_t node_id = plg.output_nodes_ids()[i];
      if (d_uni(gen) <= action_proba) {
        states[node_id] |= 1;
      } else if (states_cp[node_id] == states[node_id]) {
        states[node_id] = 0;
      }
    }
  }

  return states;
}

uint64_t MarkovBrain::current_seed() const {
  return _current_seed;
}

std::vector<uint8_t> MarkovBrain::_build_gene() {
  std::vector<uint8_t> gene;

  std::uniform_int_distribution<uint8_t> byte_uni_dist;

  gene.emplace_back(42);
  gene.emplace_back(213);

  gene.emplace_back(byte_uni_dist(_gen));
  gene.emplace_back(byte_uni_dist(_gen));

  uint32_t const nb_inputs =
      static_cast<uint32_t>(std::floor(gene[2] / 255.0 * _max_inputs));
  uint32_t const nb_outputs =
      static_cast<uint32_t>(std::floor(gene[3] / 255.0 * _max_outputs));

  uint32_t const plg_size = (1 << nb_inputs) * (1 << nb_outputs);

  for (uint32_t i = 0; i < _max_inputs; ++i) {
    gene.emplace_back(byte_uni_dist(_gen));
  }
  for (uint32_t i = 0; i < _max_outputs; ++i) {
    gene.emplace_back(byte_uni_dist(_gen));
  }

  for (uint32_t i = 0; i < plg_size; ++i) {
    gene.emplace_back(byte_uni_dist(_gen));
  }

  return gene;
}
