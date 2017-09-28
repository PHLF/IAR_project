#include "MarkovBrain.h"

using namespace sim;

MarkovBrain::MarkovBrain(MarkovBrain const& mb) {
  *this = mb;
}

MarkovBrain::MarkovBrain(MarkovBrain&& mb) {
  *this = std::move(mb);
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

MarkovBrain::MarkovBrain()
    : _max_inputs(0), _max_outputs(0), _nb_nodes(0), _nb_ancestor_genes(0) {}

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

MarkovBrain::~MarkovBrain() {
  _init_seed();
}

std::ostream& sim::operator<<(std::ostream& os, const MarkovBrain& mb) {
  os << "max_inputs " << mb._max_inputs << std::endl;
  os << "max_outputs " << mb._max_outputs << std::endl;
  os << "nb_nodes " << mb._nb_nodes << std::endl;

  for (auto const locus : mb._genome) {
    os << static_cast<uint32_t>(locus) << ' ';
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
  is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  while (std::getline(is, locus, ' ')) {
    tmp_genome.push_back(static_cast<uint8_t>(std::stoul(locus)));
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

  if (_genome.size() > 0) {
    for (uint32_t i = 0; i < _genome.size() - 1; ++i) {
      current_symbol = _genome[i];
      next_symbol = _genome[i + 1];

      if (current_symbol == 42 && next_symbol == 213) {
        _instantiate_plg(i);
      }
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
          std::round(static_cast<double>(current_symbol * _nb_nodes) / 255) -
          0.5);
    }

    increase_step(1);
  }
  for (uint32_t i = 0; i < _max_outputs; ++i) {
    if (output_nodes_ids.size() < static_cast<uint64_t>(nb_outputs)) {
      output_nodes_ids.emplace_back(
          std::round(static_cast<double>(current_symbol * _nb_nodes) / 255) -
          0.5);
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

void MarkovBrain::mutation(
    std::unordered_map<std::string, uint32_t> mut_proba) {
  std::uniform_real_distribution<double> d_proba(0, 1);

  double const proba_site_copy =
      static_cast<double>(mut_proba["proba_site_copy"]) / 1000;
  double const proba_site_del =
      static_cast<double>(mut_proba["proba_site_del"]) / 1000;
  double const proba_site_insert =
      static_cast<double>(mut_proba["proba_site_insert"]) / 1000;
  double const proba_site_replaced =
      static_cast<double>(mut_proba["proba_site_replaced"]) / 1000;
  double const proba_site_gauss_mut =
      static_cast<double>(mut_proba["proba_site_gaussian_mutation"]) / 1000;
  double const proba_gene_duplication =
      static_cast<double>(mut_proba["proba_gene_duplication"]) / 1000;
  double const proba_gene_deletion =
      static_cast<double>(mut_proba["proba_gene_deletion"]) / 1000;
  double const proba_new_gene_insert =
      static_cast<double>(mut_proba["proba_new_gene_insert"]) / 1000;

  _init_seed();

  if (_genome.size() > 0) {
    if (d_proba(_gen) <= proba_site_copy) {
      _site_copy_mutation();
    }
    if (d_proba(_gen) <= proba_site_del) {
      _site_delete_mutation();
    }
    if (d_proba(_gen) <= proba_site_insert) {
      _site_insert_mutation();
    }
    if (d_proba(_gen) <= proba_site_replaced) {
      _site_replace_mutation();
    }
    if (d_proba(_gen) <= proba_site_gauss_mut) {
      _site_gaussian_mutation();
    }
    if (d_proba(_gen) <= proba_gene_duplication) {
      _gene_duplication_mutation();
    }
    if (d_proba(_gen) <= proba_gene_deletion) {
      _gene_delete_mutation();
    }
    if (d_proba(_gen) <= proba_new_gene_insert) {
      _gene_insert_mutation();
    }

    _instantiate();
  }
}

void MarkovBrain::_site_gaussian_mutation() {
  using param = std::normal_distribution<>::param_type;
  double const sigma = 1 / 6;

  uint8_t tmp = 0;

  std::uniform_int_distribution<uint64_t> rand_index{0, _genome.size() - 1};
  std::normal_distribution<> byte_gaussian_dist{50.0, sigma};

  uint64_t index = rand_index(_gen);

  tmp = static_cast<uint8_t>(byte_gaussian_dist(
      _gen, param{static_cast<double>(_genome[index]), sigma}));
  _genome[index] = tmp;
}

void MarkovBrain::_site_copy_mutation() {
  std::uniform_int_distribution<uint64_t> rand_index{0, _genome.size() - 1};

  uint64_t index = rand_index(_gen);

  _genome.push_back(_genome[index]);
}
void MarkovBrain::_site_delete_mutation() {
  std::uniform_int_distribution<uint64_t> rand_index{0, _genome.size() - 1};

  uint64_t index = rand_index(_gen);
  _genome.erase(std::begin(_genome) + static_cast<int64_t>(index));
}

void MarkovBrain::_site_insert_mutation() {
  std::uniform_int_distribution<uint8_t> d_uni{0, 255};
  std::uniform_int_distribution<uint64_t> rand_index{0, _genome.size() - 1};

  uint64_t index = rand_index(_gen);
  _genome.insert(std::begin(_genome) + static_cast<int64_t>(index),
                 d_uni(_gen));
}

void MarkovBrain::_site_replace_mutation() {
  std::uniform_int_distribution<uint8_t> d_uni{0, 255};
  std::uniform_int_distribution<uint64_t> rand_index{0, _genome.size() - 1};

  uint64_t index = rand_index(_gen);
  _genome[index] = d_uni(_gen);
}

void MarkovBrain::_gene_insert_mutation() {
  std::uniform_int_distribution<uint64_t> rand_index{0, _genome.size() - 1};

  uint64_t index = rand_index(_gen);
  auto gene = _build_gene();

  _genome.insert(std::begin(_genome) + static_cast<int64_t>(index),
                 std::begin(gene), std::end(gene));
}

void MarkovBrain::_gene_delete_mutation() {
  std::vector<int64_t> genes_positions;
  int64_t gene_start = 0;
  int64_t gene_end = 0;

  for (uint32_t i = 0; i < _genome.size() - 1; ++i) {
    uint8_t current_symbol = _genome[i];
    uint8_t next_symbol = _genome[i + 1];

    if (current_symbol == 42 && next_symbol == 213) {
      genes_positions.emplace_back(i);
    }
  }

  if (genes_positions.size() > 2) {
    std::uniform_int_distribution<uint64_t> rand_index_genes{
        0, genes_positions.size() - 2};

    uint64_t gene_index = rand_index_genes(_gen);

    gene_start = genes_positions[gene_index];
    gene_end = genes_positions[gene_index + 1];

    _genome.erase(std::begin(_genome) + gene_start,
                  std::begin(_genome) + gene_end);
  } else if (genes_positions.size() == 2) {
    gene_start = genes_positions[0];
    gene_end = genes_positions[1];
    _genome.erase(std::begin(_genome) + gene_start,
                  std::end(_genome) + gene_end);
  }
}

void MarkovBrain::_gene_duplication_mutation() {
  std::vector<int64_t> genes_positions;
  std::uniform_int_distribution<uint64_t> rand_index{0, _genome.size() - 1};

  uint64_t index = rand_index(_gen);

  for (uint32_t i = 0; i < _genome.size() - 1; ++i) {
    uint8_t current_symbol = _genome[i];
    uint8_t next_symbol = _genome[i + 1];

    if (current_symbol == 42 && next_symbol == 213) {
      genes_positions.emplace_back(i);
    }
  }

  if (genes_positions.size() > 2) {
    std::uniform_int_distribution<uint64_t> rand_index_genes{
        0, genes_positions.size() - 2};

    uint64_t gene_index = rand_index_genes(_gen);

    int64_t gene_start = genes_positions[gene_index];
    int64_t gene_end = genes_positions[gene_index + 1];

    std::vector<uint8_t> duplicated_gene{std::begin(_genome) + gene_start,
                                         std::begin(_genome) + gene_end};

    _genome.insert(std::begin(_genome) + static_cast<int64_t>(index),
                   std::begin(duplicated_gene), std::end(duplicated_gene));
  } else if (genes_positions.size() == 2) {
    int64_t gene_start = genes_positions[0];
    int64_t gene_end = genes_positions[1];

    std::vector<uint8_t> duplicated_gene{std::begin(_genome) + gene_start,
                                         std::begin(_genome) + gene_end};

    _genome.insert(std::begin(_genome) + static_cast<int64_t>(index),
                   std::begin(duplicated_gene), std::end(duplicated_gene));
  } else if (genes_positions.size() > 0) {
    std::vector<uint8_t> duplicated_gene{std::begin(_genome),
                                         std::end(_genome)};
    _genome.insert(std::begin(_genome) + static_cast<int64_t>(index),
                   std::begin(duplicated_gene), std::end(duplicated_gene));
  }
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
