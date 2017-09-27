#ifndef MARKOVBRAIN2_H
#define MARKOVBRAIN2_H

#include "../../Misc/Globals.h"
#include "ProbaLogicGate.h"

namespace sim {
class MarkovBrain {
 public:
  MarkovBrain();
  MarkovBrain(MarkovBrain&& mb);
  MarkovBrain(MarkovBrain const& mb);
  MarkovBrain(uint32_t max_inputs,
              uint32_t max_outputs,
              uint32_t nb_nodes,
              uint32_t nb_ancestor_genes);
  ~MarkovBrain();

  friend std::ostream& operator<<(std::ostream& os, MarkovBrain const& mb);
  friend std::istream& operator>>(std::istream& is, MarkovBrain& mb);

  void generate_genome(uint64_t seed);
  void mutation();

  std::vector<uint8_t> actions(std::vector<uint8_t> state) const;

  uint64_t current_seed() const;

  MarkovBrain& operator=(MarkovBrain const& mb);
  MarkovBrain& operator=(MarkovBrain&& mb);

 private:
  uint64_t _current_seed;
  uint32_t _max_inputs;
  uint32_t _max_outputs;
  uint32_t _nb_nodes;
  uint32_t _nb_ancestor_genes;
  std::vector<uint8_t> _genome;
  std::vector<ProbabilisticLogicGate> _prob_logic_gates;
  std::mt19937 _gen;

  void _init_seed();
  void _generate_genome();
  std::vector<uint8_t> _build_gene();
  void _instantiate();
  void _instantiate_plg(uint32_t index);

  void _locus_gaussian_mutation();
  void _locus_copy_mutation();
  void _locus_delete_mutation();
  void _locus_insert_mutation();
  void _locus_replace_mutation();
  void _gene_insert_mutation();
  void _gene_delete_mutation();
  void _gene_duplication_mutation();
};
std::ostream& operator<<(std::ostream& os, MarkovBrain const& mb);
std::istream& operator>>(std::istream& is, MarkovBrain& mb);
}
#endif  // MARKOVBRAIN2_H
