#ifndef MARKOVBRAIN2_H
#define MARKOVBRAIN2_H

#include <bitset>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

#include "pcg_random.hpp"
#include "toml++/toml_table.h"

#include "ProbaLogicGate.h"

namespace sim
{
class MarkovBrain
{
  public:
    MarkovBrain();
    MarkovBrain(MarkovBrain&& mb);
    MarkovBrain(MarkovBrain const& mb);
    MarkovBrain(int max_inputs, int max_outputs, int nb_nodes,
                int nb_ancestor_genes);
    ~MarkovBrain();

    friend std::ostream& operator<<(std::ostream& os, MarkovBrain const& mb);
    friend std::istream& operator>>(std::istream& is, MarkovBrain& mb);

    static void   increase_mutation_rate();
    static void   decrease_mutation_rate();
    static double get_mutation_rate();

    void mutation(const toml::table& mutations_proba);

    template <size_t size> void actions(std::bitset<size>& ios, pcg32_fast& prng) const
    {
        std::uniform_int_distribution<uint8_t> d_uni{0, 255};

        for (auto const& plg : _prob_logic_gates)
        {
            uint32_t    state          = 0;
            const auto& input_node_ids = plg.input_nodes_ids();

            for (size_t i = 0; i < input_node_ids.size(); ++i)
            {
                // Converts state (array of booleans values) to an index (integer value)
                state |= ios[input_node_ids[i]] << i;
            }

            for (uint32_t i = 0; i < plg.nb_outputs(); ++i)
            {
                const uint8_t  action_proba = plg.table()[state * plg.nb_outputs() + i];
                const uint32_t node_id      = plg.output_nodes_ids()[i];

                if (d_uni(prng) <= action_proba)
                {
                    ios[node_id] = 1;
                }
            }
        }
    }

    uint64_t current_seed() const;

    MarkovBrain& operator=(MarkovBrain const& mb);
    MarkovBrain& operator=(MarkovBrain&& mb);

    bool operator==(const MarkovBrain& brain) const;

  private:
    static double mutation_rate;

    uint64_t                            _current_seed;
    uint32_t                            _max_inputs;
    uint32_t                            _max_outputs;
    uint32_t                            _nb_nodes;
    uint32_t                            _nb_ancestor_genes;
    std::vector<uint64_t>               _ancestors_seeds;
    std::vector<uint8_t>                _genome;
    std::vector<size_t>                 _plg_indexes;
    std::vector<ProbabilisticLogicGate> _prob_logic_gates;
    pcg64                               _gen;

    void                 _init_seed();
    void                 _generate_genome();
    std::vector<uint8_t> _build_gene();
    void                 _instantiate();
    void                 _instantiate_plg(uint32_t index);

    void _site_gaussian_mutation();
    void _site_copy_mutation();
    void _site_delete_mutation();
    void _site_insert_mutation();
    void _site_replace_mutation();
    void _gene_insert_mutation();
    void _gene_delete_mutation();
    void _gene_duplication_mutation();
};
std::ostream& operator<<(std::ostream& os, MarkovBrain const& mb);
std::istream& operator>>(std::istream& is, MarkovBrain& mb);
} // namespace sim
#endif // MARKOVBRAIN2_H
