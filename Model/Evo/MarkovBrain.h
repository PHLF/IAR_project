#ifndef MARKOVBRAIN2_H
#define MARKOVBRAIN2_H

#include <Misc/Globals.h>

namespace sim {
class MarkovBrain {
 public:
  double sigma_max = 4.4;
  double sigma = sigma_max;
  double tau = 0.02;

  MarkovBrain();
  MarkovBrain(uint32_t states, uint32_t outputs, bool feedback_loop = false);
  MarkovBrain(const MarkovBrain& markov_brain);
  MarkovBrain(MarkovBrain&& markov_brain);
  ~MarkovBrain();

  uint32_t states() const;
  uint32_t outputs() const;
  uint64_t current_seed() const;
  bool feedback_loop() const;

  MarkovBrain& operator=(MarkovBrain&& markov_brain);
  MarkovBrain& operator=(const MarkovBrain& markov_brain);

  friend std::ostream& operator<<(std::ostream& os,
                                  MarkovBrain const& markov_brain);
  friend std::istream& operator>>(std::istream& is, MarkovBrain& markov_brain);

  std::vector<uint8_t> actions(std::vector<uint8_t> state) const;
  void crossover(std::istream& is);

  void gaussian_mutation(uint64_t seed);
  void gaussian_mutation();
  void random_fill(uint64_t seed);
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
