#include <fstream>
#include <iostream>

#include "LightSim.h"

int main() {
  sim::LightSim sim;

  sim.load_settings("settings.toml");
  std::cout << sim;

  sim.sim();

  return EXIT_SUCCESS;
}
