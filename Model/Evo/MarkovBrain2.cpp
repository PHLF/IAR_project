#include "MarkovBrain2.h"

using namespace sim;

MarkovBrain2::MarkovBrain2() {}

MarkovBrain2::MarkovBrain2(uint32_t inputs,
                           uint32_t outputs,
                           bool feedback_loop)
    : _outputs(outputs), _feedback_loop(feedback_loop) {
  _states = feedback_loop ? 1 << (inputs + outputs) : 1 << inputs;

  _table = new uint8_t*[_states];

  for (uint32_t i = 0; i < _states; ++i) {
    _table[i] = new uint8_t[_outputs];
  }

  random_fill();
}

MarkovBrain2::MarkovBrain2(const MarkovBrain2& markov_brain) {
  *this = markov_brain;
}

MarkovBrain2::MarkovBrain2(MarkovBrain2&& markov_brain) {
  *this = markov_brain;
}

MarkovBrain2::~MarkovBrain2() {
  _delete_table();
}

MarkovBrain2& MarkovBrain2::operator=(const MarkovBrain2& markov_brain) {
  _delete_table();

  _states = markov_brain._states;
  _outputs = markov_brain._outputs;

  _table = new uint8_t*[_states];
  for (uint32_t i = 0; i < _states; ++i) {
    _table[i] = new uint8_t[_outputs];
  }

  for (uint32_t i = 0; i < _states; ++i) {
    for (uint32_t j = 0; j < _outputs; ++j) {
      _table[i][j] = markov_brain._table[i][j];
    }
  }

  return *this;
}

MarkovBrain2& MarkovBrain2::operator=(MarkovBrain2&& markov_brain) {
  _delete_table();

  _states = markov_brain._states;
  _outputs = markov_brain._outputs;
  _table = markov_brain._table;

  markov_brain._outputs = 0;
  markov_brain._states = 0;
  markov_brain._table = nullptr;

  return *this;
}

std::ostream& ::sim::operator<<(std::ostream& os,
                                const MarkovBrain2& markov_brain) {
  os << "states: " << markov_brain._states << std::endl;
  os << "outputs: " << markov_brain._outputs << std::endl;
  os << "feedback_loop: " << std::boolalpha << markov_brain._feedback_loop
     << std::endl;

  for (uint32_t i = 0; i < markov_brain._states; ++i) {
    for (uint32_t j = 0; j < markov_brain._outputs; ++j) {
      os << static_cast<uint32_t>(markov_brain._table[i][j]);
      if (j < markov_brain._outputs - 1) {
        os << ",";
      }
    }
    os << std::endl;
  }
  return os;
}

std::istream& ::sim::operator>>(std::istream& is, MarkovBrain2& markov_brain) {
  markov_brain._delete_table();

  is.ignore(' ');
  is >> markov_brain._states;
  is.ignore('\n');
  is.ignore(' ');
  is >> markov_brain._outputs;
  is.ignore('\n');
  is.ignore(' ');
  is >> std::boolalpha >> markov_brain._feedback_loop;
  is.ignore('\n');

  markov_brain._table = new uint8_t*[markov_brain._states];
  for (uint32_t i = 0; i < markov_brain._states; ++i) {
    markov_brain._table[i] = new uint8_t[markov_brain._outputs];
  }

  for (uint32_t i = 0; i < markov_brain._states; ++i) {
    for (uint32_t j = 0; j < markov_brain._outputs; ++j) {
      is >> markov_brain._table[i][j];
      is.ignore(1, ',');
    }
    is.ignore(1, '\n');
  }
  return is;
}

uint32_t MarkovBrain2::states() const {
  return _states;
}

uint32_t MarkovBrain2::outputs() const {
  return _outputs;
}

bool MarkovBrain2::feedback_loop() const {
  return _feedback_loop;
}

std::vector<uint8_t> MarkovBrain2::actions(std::vector<uint8_t> state) {
  std::random_device rd;
  std::ranlux24 gen(rd());
  std::uniform_int_distribution<uint8_t> uni_distrib(0, 100);

  uint32_t input_as_int = 0;
  std::vector<uint8_t> output(_outputs, 0);

  if ((1 << state.size()) == _states) {
    // vec<bool> {1, 1, ..., 1, 0} to unsigned int
    input_as_int = accumulate(state.rbegin(), state.rend(), 0,
                              [](int x, int y) { return (x << 1) + y; });

    for (uint32_t i = 0; i < _outputs; ++i) {
      if (uni_distrib(gen) <= _table[input_as_int][i]) {
        output[i] = 1;
      }
    }
  }
  return output;
}

uint64_t MarkovBrain2::random_fill() {
  uint64_t seed;
  std::random_device rd;
  std::ranlux48 gen;
  std::uniform_int_distribution<uint8_t> d_norm{0, 100};

  seed = rd();
  gen.seed(seed);

  for (uint32_t i = 0; i < _states; ++i) {
    for (uint32_t j = 0; j < _outputs; ++j) {
      _table[i][j] = d_norm(gen);
    }
  }
  return seed;
}

void MarkovBrain2::gaussian_mutation(float alpha) {
  std::random_device rd;
  std::mt19937 gen(rd());
  uint8_t tmp = 0;
  uint8_t min, max;

  for (uint32_t i = 0; i < _states; ++i) {
    for (uint32_t j = 0; j < _outputs; ++j) {
      tmp = _table[i][j];
      std::normal_distribution<> d(tmp, alpha);

      tmp = d(gen);
      max = tmp >= 0 ? tmp : 0;
      min = max <= 100 ? max : 100;

      _table[i][j] = min;
    }
  }
}

void MarkovBrain2::_delete_table() {
  if (_table != nullptr) {
    for (uint32_t i = 0; i < _states; ++i) {
      delete[] _table[i];
    }
    delete[] _table;
  };
}
