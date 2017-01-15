#include <Controleur/LightSim.h>
#include <Vue/FenetrePrincipale.h>

int main() {
  //  LightSim lightSim(512, 512, 1, 50);
  //  lightSim.run(10);

  FenetrePrincipale fen(640, 480);

  fen.render();
  SDL_Delay(5000);

  return EXIT_SUCCESS;
}
