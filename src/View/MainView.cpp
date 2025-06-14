#include "MainView.h"

#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_video.h"
#include "fmt/base.h"

#include <cstdint>
#include <exception>

using namespace sim;

MainView::MainView(int width, int height, double w_scale, double h_scale)
    : _stop(false), _render_retina(false), _render_speed(100), _width(width), _height(height),
      _w_scale_factor(w_scale), _h_scale_factor(h_scale)
{
    _init_sdl();
}

MainView::~MainView()
{
    SDL_Quit();
}

void MainView::_init_sdl()
{
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland,x11");
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fmt::println(stderr, "Unable to init SDL video: {}", SDL_GetError());
    }
    else
    {
        _init_window();
    }
}

void MainView::_init_window()
{
    _window = sim::WindowPtr(SDL_CreateWindow("Swarm simulator", SDL_WINDOWPOS_UNDEFINED,
                                              SDL_WINDOWPOS_UNDEFINED, static_cast<int32_t>(_width),
                                              static_cast<int32_t>(_height), SDL_WINDOW_SHOWN));

    if (_window.get() == nullptr)
    {
        fmt::println(stderr, "Unable to create window: {}", SDL_GetError());
    }
    else
    {
        _init_renderer();
    }
}

void MainView::_init_renderer()
{
    _renderer.reset(
        SDL_CreateRenderer(_window.get(), -1, SDL_RendererFlags::SDL_RENDERER_ACCELERATED));

    if (_renderer.get() == nullptr)
    {
        fmt::println(stderr, "Unable to instantiate renderer: {}", SDL_GetError());
        std::terminate();
    }
    else
    {
        SDL_RenderSetLogicalSize(_renderer.get(), _width, _height);
        SDL_SetRenderDrawBlendMode(_renderer.get(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(_renderer.get(), 0, 0, 0, 255);

        _load_sprites();
    }
}

void MainView::_load_sprites()
{
    _pred_sprite.reset(
        SDL_CreateTextureFromSurface(_renderer.get(), SDL_LoadBMP("rsrc/Sprites/PredatorLo.bmp")));
    _prey_sprite.reset(
        SDL_CreateTextureFromSurface(_renderer.get(), SDL_LoadBMP("rsrc/Sprites/PreyLo.bmp")));

    if (_pred_sprite.get() == nullptr || _prey_sprite.get() == nullptr)
    {
        fmt::print(stderr, "Unable to load agents sprites: {}", SDL_GetError());
    }
}

void MainView::render_clear()
{
    SDL_RenderClear(_renderer.get());
}

void MainView::render_present()
{
    SDL_SetRenderDrawColor(_renderer.get(), 255, 255, 255, 255);
    SDL_RenderPresent(_renderer.get());
}

void MainView::process_events()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        /* handle your event here */
        switch (event.type)
        {
        case SDL_MOUSEMOTION: {
            mouse_x = static_cast<int32_t>(static_cast<float>(event.motion.x) / _w_scale_factor);
            mouse_y = static_cast<int32_t>(static_cast<float>(event.motion.y) / _h_scale_factor);
            fmt::println("mouse: {}, {}", mouse_x, mouse_y);
            break;
        }
        case SDL_KEYDOWN: {
            switch (event.key.keysym.sym)
            {
            case SDLK_UP: {
                _render_speed += _render_speed / 4;

                if (_render_speed >= 2000)
                {
                    _render_speed = 2000;
                }
                fmt::println("Rendering speed: {:4}%", _render_speed);
                break;
            }
            case SDLK_DOWN: {
                _render_speed -= _render_speed / 4;

                if (_render_speed <= 10)
                {
                    _render_speed = 10;
                }
                fmt::println("Rendering speed: {:4}%", _render_speed);
                break;
            }
            case SDLK_SPACE: {
                _render_speed = 100;

                fmt::println("Rendering speed: {:4}%", _render_speed);
                break;
            }
            case SDLK_v: {
                _render_retina = !_render_retina;
                fmt::println("Render retina: {}", _render_retina ? "enabled" : "disabled");
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

bool MainView::stop_requested() const
{
    return _stop;
}

uint32_t MainView::render_speed() const
{
    return _render_speed;
}

void MainView::draw_sprite(float x_coord, float y_coord, uint16_t orientation, EDataId data_id)
{
    SDL_Texture* sprite_ptr = nullptr;
    switch (data_id)
    {
    case PREDATOR: sprite_ptr = _pred_sprite.get(); break;
    case PREY:     sprite_ptr = _prey_sprite.get(); break;
    }

    if (sprite_ptr == nullptr)
    {
        return;
    }

    int32_t sprite_w{};
    int32_t sprite_h{};

    const auto x = static_cast<int32_t>(x_coord * static_cast<float>(_w_scale_factor));
    const auto y = static_cast<int32_t>(y_coord * static_cast<float>(_h_scale_factor));

    SDL_QueryTexture(sprite_ptr, nullptr, nullptr, &sprite_w, &sprite_h);
    SDL_Rect dest{x - sprite_w / 2, y - sprite_h / 2, sprite_w, sprite_h};
    SDL_RenderCopyEx(_renderer.get(), sprite_ptr, nullptr, &dest, orientation, nullptr,
                     SDL_RendererFlip::SDL_FLIP_NONE);
}
