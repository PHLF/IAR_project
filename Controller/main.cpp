﻿#include <Controller/LightSim.h>
#include <Model/Evo/MarkovBrain.h>

int main() {
  std::ifstream myfile("settings.txt", std::ios::in);
  sim::LightSim sim;

  myfile >> sim;
  std::cout << sim;
  myfile.close();

  /*  myfile.open("pred_5_cells_36292.bin");
  sim.test_pred(myfile);
  myfile.close();*/

  sim.sim();

  return EXIT_SUCCESS;
}
