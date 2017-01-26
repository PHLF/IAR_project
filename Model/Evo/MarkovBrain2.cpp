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

  tau = markov_brain.tau;
  sigma = markov_brain.sigma;
  _states = markov_brain._states;
  _outputs = markov_brain._outputs;
  _current_seed = markov_brain._current_seed;
  _feedback_loop = markov_brain._feedback_loop;

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

  tau = markov_brain.tau;
  sigma = markov_brain.sigma;
  _states = markov_brain._states;
  _outputs = markov_brain._outputs;
  _table = markov_brain._table;
  _current_seed = markov_brain._current_seed;
  _feedback_loop = markov_brain._feedback_loop;

  markov_brain._current_seed = 0;
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
        os << " ";
      }
    }
    os << std::endl;
  }
  return os;
}

std::istream& ::sim::operator>>(std::istream& is, MarkovBrain2& markov_brain) {
  markov_brain._delete_table();
  markov_brain._outputs = 0;
  markov_brain._states = 0;

  uint32_t temp = 0;

  is.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
  is >> markov_brain._states;
  is.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
  is >> markov_brain._outputs;
  is.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
  is >> std::boolalpha >> markov_brain._feedback_loop;
  is >> std::skipws;

  markov_brain._table = new uint8_t*[markov_brain._states];
  for (uint32_t i = 0; i < markov_brain._states; ++i) {
    markov_brain._table[i] = new uint8_t[markov_brain._outputs];
  }

  for (uint32_t i = 0; i < markov_brain._states; ++i) {
    for (uint32_t j = 0; j < markov_brain._outputs; ++j) {
      is >> temp;
      markov_brain._table[i][j] = temp;
    }
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

uint64_t MarkovBrain2::current_seed() const {
  return _current_seed;
}

std::vector<uint8_t> MarkovBrain2::actions(std::vector<uint8_t> state) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint8_t> d_uni(0, 100);

  uint32_t input_as_int = 0;
  std::vector<uint8_t> output(_outputs, 0);

  if ((1 << state.size()) == _states) {
    // vec<bool> {1, 1, ..., 1, 0} to unsigned int
    input_as_int = accumulate(state.rbegin(), state.rend(), 0,
                              [](int x, int y) { return (x << 1) + y; });

    for (uint32_t i = 0; i < _outputs; ++i) {
      if (d_uni(gen) <= _table[input_as_int][i]) {
        output[i] = 1;
      }
    }
  }
  return output;
}

void MarkovBrain2::random_fill() {
  std::random_device rd;
  std::mt19937 gen;
  std::uniform_int_distribution<uint8_t> d_uni{0, 100};

  _current_seed = rd();
  gen.seed(_current_seed);

  for (uint32_t i = 0; i < _states; ++i) {
    for (uint32_t j = 0; j < _outputs; ++j) {
      _table[i][j] = d_uni(gen);
    }
  }
}

void MarkovBrain2::crossover(std::istream& is) {
  std::random_device rd;
  std::mt19937 gen;
  std::uniform_int_distribution<uint8_t> d_sigma(0, 100);

  _current_seed = rd();
  gen.seed(_current_seed);

  uint32_t temp = 0;
  uint32_t states = 0;
  uint32_t outputs = 0;
  bool feedback_loop = false;

  is.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
  is >> states;
  is.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
  is >> outputs;
  is.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
  is >> std::boolalpha >> feedback_loop;
  is >> std::skipws;

  if (states == _states && outputs == _outputs &&
      feedback_loop == _feedback_loop) {
    for (uint32_t i = 0; i < _states; ++i) {
      for (uint32_t j = 0; j < _outputs; ++j) {
        is >> temp;
        if (d_sigma(gen) <= 50) {
          _table[i][j] = temp;
        }
      }
    }
  }
}

void MarkovBrain2::gaussian_mutation() {
  std::random_device rd;
  std::mt19937 gen;
  std::normal_distribution<double> d_sigma(0, 1);
  uint8_t tmp = 0;
  uint8_t min, max;

  _current_seed = rd();
  gen.seed(_current_seed);

  // Self-adaptation
  sigma = sigma * std::exp(tau * d_sigma(gen));
  sigma = sigma > 16.6 ? 16.6 : sigma;

  for (uint32_t i = 0; i < _states; ++i) {
    for (uint32_t j = 0; j < _outputs; ++j) {
      tmp = _table[i][j];
      std::normal_distribution<> d(tmp, sigma);

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
