#include <Modele/Evo/Mn.h>

using namespace sim;

Mn::Mn(uint32_t nbAct, uint32_t nbSens)
    : _nb_actions(nbAct),
      _nb_sensors(nbSens),
      _nb_states(1 << nbSens),
      _markov_brain(_nb_states, std::vector<uint8_t>(nbAct, 50)) {
  _random_fill();
  _markov_brain.shrink_to_fit();
}

Mn::~Mn() {}

void Mn::save_as_file(std::string id) {
  std::ofstream myfile(id, std::ios::out | std::ios::binary);

  if (myfile.is_open()) {
    myfile << _nb_actions << " " << _nb_sensors << " " << _nb_states
           << std::endl;
    for (auto const& state : _markov_brain) {
      for (auto const& output : state) {
        myfile << static_cast<uint32_t>(output) << " ";
      }
      myfile << std::endl;
    }
  }
  myfile.close();
}

void Mn::load_file(std::string id) {
  std::ifstream myfile;
  std::stringstream iss;
  std::string line;
  uint32_t nb_sensors;
  uint32_t nb_actions;
  uint32_t nb_states;
  uint32_t index = 0;

  myfile.open(id, std::ios::in | std::ios::binary);

  if (myfile.is_open()) {
    std::getline(myfile, line);
    iss << line;
    iss >> nb_actions >> nb_sensors >> nb_states;

    iss.str("");
    iss.clear();

    if (nb_actions == _nb_actions && nb_sensors == _nb_sensors &&
        nb_states == _nb_states) {
      while (std::getline(myfile, line)) {
        iss << line;
        for (uint32_t i = 0; i < nb_actions; ++i) {
          iss >> _markov_brain[index][i];
        }

        iss.str("");
        iss.clear();

        index++;
      }
    } else {
      std::cout
          << "Mn non compatible avec le fichier fourni, vérifiez le nombre "
             "d'actions et de senseurs"
          << std::endl;
      std::exit(EXIT_FAILURE);
    }
    myfile.close();
  }
}

const std::vector<std::vector<uint8_t>>& Mn::markov_brain() const {
  return _markov_brain;
}

void Mn::_random_fill() {
  std::default_random_engine gen;
  std::uniform_int_distribution<uint8_t> d_norm{0, 100};

  gen.seed(std::chrono::system_clock::now().time_since_epoch().count());

  for (auto& state : _markov_brain) {
    for (auto& output : state) {
      output = d_norm(gen);
    }
  }
}

void Mn::gaussian_random_mutation(float alpha) {
  std::default_random_engine generator;
  generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

  for (auto& state : _markov_brain) {
    for (auto& output : state) {
      double tmp = output;
      tmp /= 100;
      std::normal_distribution<double> distribution(tmp, alpha);
      double tirage = distribution(generator);
      tmp = std::min(std::max(tirage, 0.0), 1.0);
      output = std::round(tmp * 100);
    }
  }
}

void Mn::print_tirages() {
  // Debug verif
  for (auto const& state : _markov_brain) {
    for (auto const& output : state) {
      std::cout << unsigned(output) << ", ";
    }
    std::cout << std::endl;
  }
}

std::vector<uint8_t> Mn::choose_action(std::vector<uint8_t> input) {
  std::default_random_engine gen;
  std::uniform_int_distribution<uint8_t> distrib_norm(0, 100);
  gen.seed(std::chrono::system_clock::now().time_since_epoch().count());

  uint32_t input_as_int = 0;
  std::vector<uint8_t> output(_nb_actions, 0);

  // vec<bool> {1, 1, ..., 1, 0} to unsigned int
  input_as_int = accumulate(input.rbegin(), input.rend(), 0,
                            [](int x, int y) { return (x << 1) + y; });

  for (uint32_t i = 0; i < _nb_actions; ++i) {
    if (distrib_norm(gen) <= _markov_brain[input_as_int][i]) {
      output[i] = 1;
    }
  }
  return output;
}
