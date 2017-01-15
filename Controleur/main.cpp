#include "LightSim.h"

using namespace std;

int main() {
  LightSim lightSim(512, 512, 1, 50);
  lightSim.run(10);

  return EXIT_SUCCESS;
}
