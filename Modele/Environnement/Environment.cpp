#include <Modele/Environnement/Environment.h>

using namespace sim;

Environment::Environment(uint32_t x,
                         uint32_t y,
                         uint32_t nb_predators,
                         uint32_t nb_preys)
    : size_x(x), size_y(y),_nb_predators(nb_predators),_nb_preys(nb_preys) {
  // ctor

  for (uint32_t i = 0; i < nb_predators; ++i) {
    _agents.emplace_back(std::unique_ptr<Agent>(new Predator()));
  }
  for (uint32_t i = 0; i < nb_preys; ++i) {
    _agents.emplace_back(std::unique_ptr<Agent>(new Prey()));
  }
}

Environment::~Environment() {}

uint32_t Environment::getNb_predators() const
{
    return _nb_predators;
}

uint32_t Environment::getNb_preys() const
{
    return _nb_preys;
}
