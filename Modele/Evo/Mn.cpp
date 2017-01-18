#include <Modele/Evo/Mn.h>

Mn::Mn(uint32_t nbAct, uint32_t nbSens)
    : _nb_actions(nbAct),
      _nb_sensors(nbSens),
      _nb_states(std::pow(2, nbAct + nbSens)),
      _markov_brain(_nb_states, std::vector<float>(nbAct, 0.5)) {}

Mn::~Mn() {}

void Mn::save_as_file(std::string id) {
  std::ofstream myfile(id);

  if (myfile.is_open()) {
    myfile << _nb_actions << " " << _nb_sensors << " " << _nb_states
           << std::endl;
    for (auto const& state : _markov_brain) {
      for (auto const& output : state) {
        myfile << output << " ";
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

  myfile.open(id);

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

const std::vector<std::vector<float>>& Mn::markov_brain() const {
  return _markov_brain;
}

void Mn::random_fill() {
  std::default_random_engine gen;
  std::uniform_real_distribution<float> d_norm{0.0, 1.0};

  gen.seed(std::chrono::system_clock::now().time_since_epoch().count());

  for (auto& state : _markov_brain) {
    for (auto& output : state) {
      output = d_norm(gen);
    }
  }
}

void Mn::print_tirages() {
  // Debug verif

  for (auto const& state : _markov_brain) {
    for (auto const& output : state) {
      std::cout << output << ", ";
    }
    std::cout << std::endl;
  }
}

std::vector<uint8_t> Mn::choose_action(std::vector<uint8_t> input) {
  std::default_random_engine gen;
  std::uniform_real_distribution<float> distrib_norm(0.0, 1.0);
  gen.seed(std::chrono::system_clock::now().time_since_epoch().count());

  float tirage = 0.0;
  uint32_t input_as_int = 0;
  std::vector<uint8_t> output(_nb_actions, 0);

  // vec<bool> {1, 1, ..., 1, 0} to unsigned int
  input_as_int = accumulate(input.rbegin(), input.rend(), 0,
                            [](int x, int y) { return (x << 1) + y; });

  for (uint32_t i = 0; i < _nb_actions; ++i) {
    tirage = distrib_norm(gen);
    std::cout << "tirage : " << tirage << std::endl;
    if (tirage <= _markov_brain[input_as_int][i]) {
      output[i] = 1;
    }
  }
  return output;
}
