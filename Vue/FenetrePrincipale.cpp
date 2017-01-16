#include <Vue/FenetrePrincipale.h>

FenetrePrincipale::FenetrePrincipale(u_int32_t width, u_int32_t height)
    : _width(width), _height(height) {
  _init_sdl();
}

FenetrePrincipale::~FenetrePrincipale() {
  SDL_Quit();
}

void FenetrePrincipale::render() {
  if (_renderer.get() != nullptr) {
    SDL_RenderClear(_renderer.get());
    _render_agents();
    SDL_RenderPresent(_renderer.get());
  }
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
  _window = sdl2::WindowPtr(SDL_CreateWindow(
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

void FenetrePrincipale::_render_agents() {
  int32_t w;
  int32_t h;
  SDL_Texture* sprite_ptr = nullptr;

  for (const auto& agent_view : agents_views) {
    sprite_ptr = agent_view.prey ? _prey_sprite.get() : _pred_sprite.get();

    SDL_QueryTexture(sprite_ptr, nullptr, nullptr, &w, &h);
    SDL_Rect dest = {agent_view.x - w / 2, agent_view.y - h / 2, w, h};
    SDL_RenderCopyEx(_renderer.get(), sprite_ptr, nullptr, &dest,
                     agent_view.angle, nullptr,
                     SDL_RendererFlip::SDL_FLIP_NONE);
  }
}
