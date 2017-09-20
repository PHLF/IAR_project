#include "MarkovBrain_copy.h"

using namespace sim;

MarkovBrain::MarkovBrain(uint32_t max_inputs,
                         uint32_t max_outputs,
                         uint32_t nb_nodes,
                         uint32_t nb_ancestor_genes)
    : _max_inputs(max_inputs),
      _max_outputs(max_outputs),
      _nb_nodes(nb_nodes),
      _nb_ancestor_genes(nb_ancestor_genes) {
  init_genome();
}

void MarkovBrain::_build_from_genome() {
  std::vector<uint32_t> genes_start_positions;
  _build_plg(0, genes_start_positions);
}

void MarkovBrain::_build_plg(uint32_t index,
                             std::vector<uint32_t>& genes_start_positions) {
  bool done = false;
  uint8_t current_symbol;
  uint8_t next_symbol;
  int32_t nb_inputs = -1;
  int32_t nb_outputs = -1;
  std::vector<uint32_t> input_nodes_ids;
  std::vector<uint32_t> output_nodes_ids;
  std::vector<uint8_t> table;

  std::vector<uint8_t> const& genome = _genome;
  uint64_t const genome_length = genome.size();

  auto increase_and_check_start_codon = [&, this](uint32_t step) {
    index = (index + step) % (genome_length - 1);

    current_symbol = genome[index];
    next_symbol = genome[index + 1];

    if (current_symbol == 42 && next_symbol == 213) {
      if (std::find(std::cbegin(genes_start_positions),
                    std::cend(genes_start_positions),
                    index) == std::cend(genes_start_positions)) {
        genes_start_positions.push_back(index);
        _build_plg(index, genes_start_positions);
      }
    }
  };

  increase_and_check_start_codon(2);

  if (nb_inputs == -1) {
    nb_inputs =
        static_cast<int32_t>(std::floor(current_symbol / 255.0 * _max_inputs));
    nb_outputs =
        static_cast<int32_t>(std::floor(next_symbol / 255.0 * _max_outputs));

    increase_and_check_start_codon(2);
  } else if (input_nodes_ids.empty()) {
    for (uint32_t i = 0; i < _max_inputs; ++i) {
      if (input_nodes_ids.size() <= static_cast<uint64_t>(nb_inputs)) {
        input_nodes_ids.emplace_back(
            std::lround((current_symbol * _nb_nodes) / 255.0 - 0.5));
      }

      increase_and_check_start_codon(1);
    }
  } else if (output_nodes_ids.empty()) {
    for (uint32_t i = 0; i < _max_outputs; ++i) {
      if (output_nodes_ids.size() <= static_cast<uint64_t>(nb_outputs)) {
        output_nodes_ids.emplace_back(
            std::lround((current_symbol * _nb_nodes) / 255.0 - 0.5));
      }

      increase_and_check_start_codon(1);
    }
  }
  while (!done) {
    table.push_back(current_symbol);
    table.push_back(next_symbol);

    if (table.size() ==
        static_cast<uint64_t>((2 << nb_inputs) * (2 << nb_outputs))) {
      done = true;
    } else {
      increase_and_check_start_codon(2);
    }
  }
}

void MarkovBrain::init_genome() {
  std::random_device rd;

  _current_seed = rd();

  for (uint32_t i = 0; i < _nb_ancestor_genes; ++i) {
    auto gene = _build_gene();
    _genome.insert(std::begin(_genome), std::begin(gene), std::end(gene));
  }
  _genome.shrink_to_fit();
}

void MarkovBrain::init_genome(uint64_t seed) {
  _current_seed = seed;

  for (uint32_t i = 0; i < _nb_ancestor_genes; ++i) {
    auto gene = _build_gene();
    _genome.insert(std::begin(_genome), std::begin(gene), std::end(gene));
  }
  _genome.shrink_to_fit();
}

std::vector<uint8_t> MarkovBrain::_build_gene() {
  std::vector<uint8_t> gene;
  std::mt19937 gen;
  std::uniform_int_distribution<uint8_t> byte_uni_dist;

  gen.seed(_current_seed);

  gene.emplace_back(42);
  gene.emplace_back(213);

  gene.emplace_back(byte_uni_dist(gen));
  gene.emplace_back(byte_uni_dist(gen));

  uint8_t const& byte_nb_inputs = gene[2];
  uint8_t const& byte_nb_outputs = gene[3];

  uint32_t const nb_inputs =
      static_cast<uint32_t>(std::floor(byte_nb_inputs / 255.0 * _max_inputs));
  uint32_t const nb_outputs =
      static_cast<uint32_t>(std::floor(byte_nb_outputs / 255.0 * _max_outputs));

  uint32_t const plg_size = (2u << nb_inputs) * (2u << nb_outputs);

  for (uint32_t i; i < _max_inputs; ++i) {
    gene.emplace_back(byte_uni_dist(gen));
  }
  for (uint32_t i; i < _max_outputs; ++i) {
    gene.emplace_back(byte_uni_dist(gen));
  }

  for (uint32_t i; i < plg_size; ++i) {
    gene.emplace_back(byte_uni_dist(gen));
  }

  return gene;
}
