#include "MainView.h"

#include "Model/Agents/Retina.h"

#include "fmt/printf.h"

using namespace sim;

MainView::MainView(u_int32_t width,
                   u_int32_t height,
                   double w_scale,
                   double h_scale)
    : _stop(false),
      _render_retina(false),
      _render_speed(100),
      _agents(nullptr),
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
    fmt::print(std::cerr, "Unable to init SDL video: {}\n", SDL_GetError());
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
    fmt::print(std::cerr, "Unable to create window: ", SDL_GetError());
  } else {
    _init_renderer();
  }
}

void MainView::_init_renderer() {
  _renderer.reset(SDL_CreateRenderer(
      _window.get(), -1, SDL_RendererFlags::SDL_RENDERER_ACCELERATED));

  if (_renderer.get() == nullptr) {
    fmt::print(std::cerr, "Unable to instantiate renderer: ", SDL_GetError());
  } else {
    SDL_RenderSetLogicalSize(_renderer.get(), static_cast<int32_t>(_width),
                             static_cast<int32_t>(_height));
    SDL_SetRenderDrawBlendMode(_renderer.get(), SDL_BLENDMODE_BLEND);
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
    fmt::print(std::cerr, "Unable to load agents sprites: ", SDL_GetError());
  }
}

void MainView::render() {
  if (_renderer.get() != nullptr) {
    SDL_RenderClear(_renderer.get());
    _render_agents();
    SDL_RenderPresent(_renderer.get());
  }
}

void MainView::process_events() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    /* handle your event here */
    switch (event.type) {
      case SDL_KEYDOWN: {
        switch (event.key.keysym.sym) {
          case SDLK_f: {
            _render_speed += _render_speed / 4;

            if (_render_speed >= 2000) {
              _render_speed = 2000;
            }
            fmt::print("Rendering speed: {:4}%\n", _render_speed);
            break;
          }
          case SDLK_s: {
            _render_speed -= _render_speed / 4;

            if (_render_speed <= 100) {
              _render_speed = 100;
            }
            fmt::print("Rendering speed: {:4}%\n", _render_speed);
            break;
          }
          case SDLK_v: {
            _render_retina = !_render_retina;
            fmt::print("Render retina: {}\n",
                       _render_retina ? "enabled" : "disabled");
            break;
          }
        }
        break;
      }
      case SDL_QUIT: {
        _stop = true;
      }
    }
  }
}

const TexturePtr& MainView::pred_sprite() const {
  return _pred_sprite;
}

const TexturePtr& MainView::prey_sprite() const {
  return _prey_sprite;
}

void MainView::set_agents(const Agents* agents) {
  _agents = agents;
}

bool MainView::stop_requested() const {
  return _stop;
}

uint32_t MainView::render_speed() const {
  return _render_speed;
}

void MainView::_render_agents() {
  int32_t w;
  int32_t h;

  SDL_Texture* sprite_ptr = nullptr;

  thread_local std::array<SDL_Point, 4> points;

  for (auto const& agent : *_agents) {
    sprite_ptr = agent->get_sprite();

    static constexpr sim::Color view_vector_color{220, 220, 220, 50};

    if (sprite_ptr == nullptr) {
      continue;
    }

    const auto x = static_cast<int32_t>(static_cast<float>(agent->coords.x) *
                                        static_cast<float>(_w_scale_factor));
    const auto y = static_cast<int32_t>(static_cast<float>(agent->coords.y) *
                                        static_cast<float>(_h_scale_factor));

    SDL_QueryTexture(sprite_ptr, nullptr, nullptr, &w, &h);
    SDL_Rect dest{x - w / 2, y - h / 2, w, h};
    SDL_RenderCopyEx(_renderer.get(), sprite_ptr, nullptr, &dest,
                     agent->orientation, nullptr,
                     SDL_RendererFlip::SDL_FLIP_NONE);

    if (_render_retina) {
      const auto [layers, nb_layers] = agent->retina().layers();
      for (size_t i = 0; i < nb_layers; ++i) {
        for (const auto& cell : layers[i]->cells()) {
          SDL_SetRenderDrawColor(
              _renderer.get(), view_vector_color.red, view_vector_color.green,
              view_vector_color.blue, view_vector_color.alpha);
          const auto left_x =
              x + static_cast<int32_t>(static_cast<float>(cell.left_vector.x) *
                                       static_cast<float>(_w_scale_factor));
          const auto left_y =
              y + static_cast<int32_t>(static_cast<float>(cell.left_vector.y) *
                                       static_cast<float>(_h_scale_factor));

          const auto right_x =
              x + static_cast<int32_t>(static_cast<float>(cell.right_vector.x) *
                                       static_cast<float>(_w_scale_factor));
          const auto right_y =
              y + static_cast<int32_t>(static_cast<float>(cell.right_vector.y) *
                                       static_cast<float>(_h_scale_factor));

          if (cell.target != nullptr) {
            const auto color = cell.target->color();
            SDL_SetRenderDrawColor(_renderer.get(), color.red, color.green,
                                   color.blue, color.alpha / 2);
            //    const SDL_Rect test{(left_x + right_x + x) / 3,
            //                        (left_y + right_y + y) / 3, 8, 8};

            //    SDL_RenderDrawRect(_renderer.get(), &test);
          }

          points[0] = {x, y};
          points[1] = {left_x, left_y};
          points[2] = {right_x, right_y};
          points[3] = {x, y};

          SDL_RenderDrawLines(_renderer.get(), points.data(),
                              static_cast<int32_t>(points.size()));
        }
      }
    }
  }
  SDL_SetRenderDrawColor(_renderer.get(), 255, 255, 255, 255);
}
