#include <Vue/FenetrePrincipale.h>

using namespace sim;

FenetrePrincipale::FenetrePrincipale(u_int32_t width,
                                     u_int32_t height,
                                     double w_scale,
                                     double h_scale,
                                     Agents& agents)
    : _agents(agents),
      _width(width),
      _height(height),
      _w_scale_factor(w_scale),
      _h_scale_factor(h_scale) {
  _init_sdl();
}

FenetrePrincipale::~FenetrePrincipale() {
  SDL_Quit();
}

void FenetrePrincipale::_init_sdl() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Impossible d'initialiser SDL: " << SDL_GetError()
              << std::endl;
  } else {
    _init_window();
  }
}

void FenetrePrincipale::_init_window() {
  _window = sim::WindowPtr(SDL_CreateWindow(
      "Swarm simulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      _width, _height, SDL_WINDOW_SHOWN));

  if (_window.get() == nullptr) {
    std::cerr << "Impossible de créer la fenêtre principale: " << SDL_GetError()
              << std::endl;
  } else {
    _init_main_surface();
  }
}

void FenetrePrincipale::_init_renderer() {
  _renderer.reset(SDL_CreateRenderer(
      _window.get(), -1, SDL_RendererFlags::SDL_RENDERER_ACCELERATED));

  if (_renderer.get() == nullptr) {
    std::cerr << "Impossible de créer le moteur de rendu: " << SDL_GetError()
              << std::endl;
  } else {
    SDL_RenderSetLogicalSize(_renderer.get(), _width, _height);
    SDL_SetRenderDrawColor(_renderer.get(), 0, 0, 0, 255);

    _load_sprites();
  }
}

void FenetrePrincipale::_load_sprites() {
  _pred_sprite.reset(SDL_CreateTextureFromSurface(
      _renderer.get(), SDL_LoadBMP("Ressources/Sprites/PredatorLo.bmp")));
  _prey_sprite.reset(SDL_CreateTextureFromSurface(
      _renderer.get(), SDL_LoadBMP("Ressources/Sprites/PreyLo.bmp")));

  if (_pred_sprite.get() == nullptr || _prey_sprite.get() == nullptr) {
    std::cerr << "Impossible de charger les images des agents: "
              << SDL_GetError() << std::endl;
  }
}

void FenetrePrincipale::_init_main_surface() {
  _win_surface.reset(SDL_GetWindowSurface(_window.get()));

  if (_win_surface.get() == nullptr) {
    std::cerr << "Impossible de créer la surface de rendu de la fenêtre: "
              << SDL_GetError() << std::endl;
  } else {
    _init_renderer();
  }
}

void FenetrePrincipale::render() {
  if (_renderer.get() != nullptr) {
    SDL_RenderClear(_renderer.get());
    _render_agents();
    SDL_RenderPresent(_renderer.get());
  }
}

void FenetrePrincipale::_render_agents() {
  int32_t w;
  int32_t h;
  int32_t x;
  int32_t y;

  SDL_Texture* sprite_ptr = nullptr;
  SDL_SetRenderDrawColor(_renderer.get(), 255, 0, 0, 255);

  std::vector<SDL_Point> points;

  for (const auto& agent : _agents) {
    sprite_ptr = agent->predates ? _pred_sprite.get() : _prey_sprite.get();

    x = std::round(agent->get_coord().x * _w_scale_factor);
    y = std::round(agent->get_coord().y * _h_scale_factor);

    SDL_QueryTexture(sprite_ptr, nullptr, nullptr, &w, &h);
    SDL_Rect dest{x - w / 2, y - h / 2, w, h};
    SDL_RenderCopyEx(_renderer.get(), sprite_ptr, nullptr, &dest,
                     agent->get_orientation(), nullptr,
                     SDL_RendererFlip::SDL_FLIP_NONE);

    points.push_back(SDL_Point{x, y});
    for (const auto& vec : agent->get_retina()->get_view_vectors()) {
      int32_t x_point = x + vec.x * _w_scale_factor;
      int32_t y_point = y + vec.y * _h_scale_factor;

      points.push_back(SDL_Point{x_point, y_point});
    }
    points.push_back(SDL_Point{x, y});
    SDL_RenderDrawLines(_renderer.get(), points.data(), points.size());
    points.clear();
  }
  SDL_SetRenderDrawColor(_renderer.get(), 0, 0, 0, 255);
}
