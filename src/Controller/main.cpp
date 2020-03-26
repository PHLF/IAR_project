#include <fstream>
#include <iostream>

#include "EvOverseer.h"

int main() {
  sim::EvOverseer sim;

  sim.load_settings("settings.toml");
  std::cout << sim;

  sim.sim();

  return EXIT_SUCCESS;
}
