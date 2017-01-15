#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <Vue/SDLWrappers.h>

class FenetrePrincipale {
 public:
  FenetrePrincipale(u_int32_t width, u_int32_t height);
  ~FenetrePrincipale();

  void render();

 private:
  sdl2::WindowPtr _window;
  sdl2::RendererPtr _renderer;

  u_int32_t _width;
  u_int32_t _height;

  void _init_sdl();
  void _init_window();
  void _init_renderer();
};

#endif  // FENETREPRINCIPALE_H
