#ifndef MARKOVBRAIN2_H
#define MARKOVBRAIN2_H

#include <Misc/Globals.h>

namespace sim {
class MarkovBrain2 {
 public:
  double sigma = 16.6;
  double tau = 0.1;

  MarkovBrain2();
  MarkovBrain2(uint32_t states, uint32_t outputs, bool feedback_loop = false);
  MarkovBrain2(const MarkovBrain2& markov_brain);
  MarkovBrain2(MarkovBrain2&& markov_brain);
  ~MarkovBrain2();

  uint32_t states() const;
  uint32_t outputs() const;
  uint64_t current_seed() const;
  bool feedback_loop() const;

  MarkovBrain2& operator=(MarkovBrain2&& markov_brain);
  MarkovBrain2& operator=(const MarkovBrain2& markov_brain);

  friend std::ostream& operator<<(std::ostream& os,
                                  MarkovBrain2 const& markov_brain);
  friend std::istream& operator>>(std::istream& is, MarkovBrain2& markov_brain);

  std::vector<uint8_t> actions(std::vector<uint8_t> state) const;
  void crossover(std::istream& is);

  void gaussian_mutation(uint64_t seed);
  void gaussian_mutation();
  void random_fill();

  void self_adaptation();

 private:
  uint8_t* _table = nullptr;

  uint32_t _states = 0;
  uint32_t _outputs = 0;
  uint64_t _current_seed = 0;

  bool _feedback_loop = false;

  void _delete_table();
};
}
#endif  // MARKOVBRAIN2_H
