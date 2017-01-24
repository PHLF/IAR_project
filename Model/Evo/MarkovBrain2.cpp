#include "MarkovBrain2.h"

MarkovBrain2::MarkovBrain2() {}

MarkovBrain2::MarkovBrain2(uint32_t states,
                           uint32_t outputs,
                           bool feedback_loop = false)
    : _outputs(outputs) {
  if (feedback_loop) {
    _states = states + outputs;
  }

  _table = new uint8_t*[_states];

  for (uint32_t i = 0; i < _states; ++i) {
    _table[i] = new uint8_t[_outputs];
  }
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
    _table = new uint8_t[_outputs];
  }

  for (uint32_t i = 0; i < _states; ++i) {
    for (uint32_t j = 0; j < _outputs; ++j) {
      _table[i][j] = _markov_brain._table[i][j];
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

ostream& MarkovBrain2::operator<<(ostream& os,
                                  const MarkovBrain2& markov_brain) {
  os << "states: " << markov_brain._states << std::endl;
  os << "outputs: " << markov_brain._outputs << std::endl;

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

istream& MarkovBrain2::operator>>(istream& is, MarkovBrain2& markov_brain) {
  markov_brain._delete_table();

  is.ignore(1, ' ');
  is >> markov_brain._states;
  is.ignore(1, '\n');
  is.ignore(1, ' ');
  markov_brain._outputs;
  is.ignore(1, '\n');

  _table = new uint8_t*[markov_brain._states];
  for (uint32_t i = 0; i < _states; ++i) {
    _table = new uint8_t[markov_brain._outputs];
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
