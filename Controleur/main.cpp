#include <Controleur/LightSim.h>
#include <Vue/FenetrePrincipale.h>

extern double degToRad(int ori);

int main() {
  //  LightSim lightSim(512, 512, 1, 50);
  //  lightSim.run(10);

  double win_w = 1024;
  double win_h = 768;
  double grid_w = 512;
  double grid_h = 512;

  AgentView test;
  test.x = win_w / 2;
  test.y = win_h / 2;
  test.angle = 45;
  test.prey = true;

  double x_f = test.x;
  double y_f = test.y;

  FenetrePrincipale fen(win_w, win_h);

  fen.agents_views.emplace_back(test);

  auto& a = fen.agents_views[0];

  for (uint32_t i = 0; i < 1e5; ++i) {
    a.angle += 1;
    x_f = x_f + std::cos(degToRad(a.angle)) * (win_w / grid_w);
    y_f = y_f + std::sin(degToRad(a.angle)) * (win_h / grid_h);
    a.x = std::round(x_f);
    a.y = std::round(y_f);
    fen.render();
    SDL_Delay(16);
  }

  return EXIT_SUCCESS;
}
