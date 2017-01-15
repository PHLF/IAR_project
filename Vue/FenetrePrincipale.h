#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <iostream>
#include <SDL2/SDL.h>
#include "Misc/Globals.h"

class FenetrePrincipale {
 public:
  FenetrePrincipale();

 private:
  std::unique_ptr<SDL_Window> _window;
  std::unique_ptr<SDL_Surface> _screenSurface;
};

#endif  // FENETREPRINCIPALE_H
