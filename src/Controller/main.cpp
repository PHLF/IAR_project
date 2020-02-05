#include <fstream>
#include <iostream>

#include "LightSim.h"

int main() {
  std::ifstream myfile("settings.txt", std::ios::in);
  sim::LightSim sim;

  myfile >> sim;
  std::cout << sim;
  myfile.close();

  sim.sim();

  return EXIT_SUCCESS;
}
