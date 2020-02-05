#include "MainView.h"

using namespace sim;

MainView::MainView(u_int32_t width,
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

MainView::~MainView() {
  SDL_Quit();
}

void MainView::_init_sdl() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Impossible d'initialiser SDL: " << SDL_GetError()
              << std::endl;
  } else {
    _init_window();
  }
}

void MainView::_init_window() {
  _window = sim::WindowPtr(
      SDL_CreateWindow("Swarm simulator", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, static_cast<int32_t>(_width),
                       static_cast<int32_t>(_height), SDL_WINDOW_SHOWN));

  if (_window.get() == nullptr) {
    std::cerr << "Impossible de créer la fenêtre principale: " << SDL_GetError()
              << std::endl;
  } else {
    _init_renderer();
  }
}

void MainView::_init_renderer() {
  _renderer.reset(SDL_CreateRenderer(
      _window.get(), -1, SDL_RendererFlags::SDL_RENDERER_ACCELERATED));

  if (_renderer.get() == nullptr) {
    std::cerr << "Impossible de créer le moteur de rendu: " << SDL_GetError()
              << std::endl;
  } else {
    SDL_RenderSetLogicalSize(_renderer.get(), static_cast<int32_t>(_width),
                             static_cast<int32_t>(_height));
    SDL_SetRenderDrawColor(_renderer.get(), 0, 0, 0, 255);

    _load_sprites();
  }
}

void MainView::_load_sprites() {
  _pred_sprite.reset(SDL_CreateTextureFromSurface(
      _renderer.get(), SDL_LoadBMP("rsrc/Sprites/PredatorLo.bmp")));
  _prey_sprite.reset(SDL_CreateTextureFromSurface(
      _renderer.get(), SDL_LoadBMP("rsrc/Sprites/PreyLo.bmp")));

  if (_pred_sprite.get() == nullptr || _prey_sprite.get() == nullptr) {
    std::cerr << "Impossible de charger les images des agents: "
              << SDL_GetError() << std::endl;
  }
}

void MainView::render() {
  if (_renderer.get() != nullptr) {
    SDL_RenderClear(_renderer.get());
    _render_agents();
    SDL_RenderPresent(_renderer.get());
  }
}

const TexturePtr& MainView::pred_sprite() const {
  return _pred_sprite;
}

const TexturePtr& MainView::prey_sprite() const {
  return _prey_sprite;
}

void MainView::_render_agents() {
  int32_t w;
  int32_t h;
  int32_t x;
  int32_t y;

  SDL_Texture* sprite_ptr = nullptr;

  std::vector<SDL_Point> points;

  for (auto const& agent : _agents) {
    auto color = agent->color();

    sprite_ptr = agent->get_sprite();

    SDL_SetRenderDrawColor(_renderer.get(), color.red, color.green, color.blue,
                           color.alpha);

    x = static_cast<int32_t>(std::round(agent->coord.x * _w_scale_factor));
    y = static_cast<int32_t>(std::round(agent->coord.y * _h_scale_factor));

    SDL_QueryTexture(sprite_ptr, nullptr, nullptr, &w, &h);
    SDL_Rect dest{x - w / 2, y - h / 2, w, h};
    SDL_RenderCopyEx(_renderer.get(), sprite_ptr, nullptr, &dest,
                     agent->orientation, nullptr,
                     SDL_RendererFlip::SDL_FLIP_NONE);

    points.push_back(SDL_Point{x, y});
    for (auto const& vec : agent->retina().view_vectors()) {
      int32_t x_point =
          static_cast<int32_t>(std::round(x + vec.x * _w_scale_factor));
      int32_t y_point =
          static_cast<int32_t>(std::round(y + vec.y * _h_scale_factor));

      points.push_back(SDL_Point{x_point, y_point});
    }
    points.push_back(SDL_Point{x, y});
    SDL_RenderDrawLines(_renderer.get(), points.data(),
                        static_cast<int32_t>(points.size()));
    points.clear();
  }
  SDL_SetRenderDrawColor(_renderer.get(), 255, 255, 255, 255);
}
