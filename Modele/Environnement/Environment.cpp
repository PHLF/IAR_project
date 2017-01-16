#include <Modele/Environnement/Environment.h>

Environment::Environment(uint32_t x,
                         uint32_t y,
                         uint32_t nb_predators,
                         uint32_t nb_preys)
    : _size_x(x), _size_y(y) {
  // ctor

  for (uint32_t i = 0; i < nb_predators; ++i) {
    _predators.emplace_back(std::unique_ptr<Agent>(new Predator()));
  }
  for (uint32_t i = 0; i < nb_preys; ++i) {
    _preys.emplace_back(std::unique_ptr<Agent>(new Prey()));
  }

}

Environment::~Environment() {}
