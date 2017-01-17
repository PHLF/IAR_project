#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <Vue/SDLWrappers.h>
#include <Modele/Agents/Agent.h>

namespace sim {

class FenetrePrincipale {
 public:
  FenetrePrincipale(u_int32_t width,
                    u_int32_t height,
                    double w_scale,
                    double h_scale,
                    Agents& agents);
  ~FenetrePrincipale();

  void render();

 private:
  const Agents& _agents;

  WindowPtr _window;
  RendererPtr _renderer;

  TexturePtr _prey_sprite;
  TexturePtr _pred_sprite;

  const uint32_t _width;
  const uint32_t _height;

  const double _w_scale_factor;
  const double _h_scale_factor;

  void _init_sdl();
  void _init_window();
  void _init_renderer();

  void _render_agents();
  void _load_sprites();
};
}
#endif  // FENETREPRINCIPALE_H
