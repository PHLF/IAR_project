#ifndef MARKOVBRAIN2_H
#define MARKOVBRAIN2_H

#include <Misc/Globals.h>

class MarkovBrain2 {
 public:
  MarkovBrain2();
  MarkovBrain2(uint32_t states, uint32_t outputs);
  MarkovBrain2(const MarkovBrain2& markov_brain);
  MarkovBrain2(MarkovBrain2&& markov_brain);
  ~MarkovBrain2();

  uint32_t states() const;
  uint32_t outputs() const;

  MarkovBrain2& operator=(MarkovBrain2& markov_brain);
  MarkovBrain2& operator=(const MarkovBrain2& markov_brain);

  friend ostream& operator<<(ostream& os, MarkovBrain2 const& markov_brain);
  friend istream& operator>>(istream& is, MarkovBrain2& markov_brain);
private:
  uint8_t** _table = nullptr;

  uint32_t _states = 0;
  uint32_t _outputs = 0;

  void _delete_table();
};

#endif  // MARKOVBRAIN2_H
