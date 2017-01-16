#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <Vue/SDLWrappers.h>

class AgentView {
 public:
  int32_t x;
  int32_t y;
  float angle;
  bool prey;
};

class FenetrePrincipale {
 public:
  std::vector<AgentView> agents_views;

  FenetrePrincipale(u_int32_t width, u_int32_t height);
  ~FenetrePrincipale();

  void render();

 private:
  sdl2::WindowPtr _window;
  sdl2::SurfacePtr _win_surface;
  sdl2::RendererPtr _renderer;

  sdl2::TexturePtr _prey_sprite;
  sdl2::TexturePtr _pred_sprite;

  u_int32_t _width;
  u_int32_t _height;

  void _init_sdl();
  void _init_window();
  void _init_renderer();
  void _init_main_surface();

  void _render_agents();
  void _load_sprites();
};

#endif  // FENETREPRINCIPALE_H
