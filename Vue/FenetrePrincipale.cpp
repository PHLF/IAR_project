#include "FenetrePrincipale.h"

FenetrePrincipale::FenetrePrincipale()
    : _window(nullptr), _screenSurface(nullptr) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Impossible d'initialiser SDL: " << SDL_GetError();
  }
}
