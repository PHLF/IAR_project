#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include "Misc/Utils.h"

#include "SDLWrappers.h"
#include "SDL_rect.h"
#include <cstdint>

namespace sim
{

class MainView
{
  public:
    enum EDataId : uint8_t
    {
        PREDATOR,
        PREY,
    };

    MainView(u_int32_t width, u_int32_t height, double w_scale, double h_scale);
    ~MainView();

    void render_clear();
    void render_present();
    void process_events();

    bool stop_requested() const;

    uint32_t render_speed() const;

    void draw_sprite(float x_coord, float y_coord, uint16_t orientation, EDataId data_id);
    void draw_line(SDL_Point start, SDL_Point end, Color color)
    {
        if (!_render_retina)
        {
            return;
        }

        SDL_SetRenderDrawColor(_renderer.get(), color.red, color.green, color.blue, color.alpha);
        SDL_RenderDrawLine(_renderer.get(),
                           static_cast<int32_t>(start.x * _w_scale_factor),
                           static_cast<int32_t>(start.y * _h_scale_factor),
                           static_cast<int32_t>(end.x * _w_scale_factor),
                           static_cast<int32_t>(end.y * _h_scale_factor));
    }

    auto mouse_coords() const -> std::array<int32_t, 2> { return {mouse_x, mouse_y}; }

  private:
    bool     _stop;
    bool     _render_retina;
    uint32_t _render_speed;

    WindowPtr   _window;
    RendererPtr _renderer;

    TexturePtr _prey_sprite;
    TexturePtr _pred_sprite;

    const uint32_t _width;
    const uint32_t _height;

    const ffloat _w_scale_factor;
    const ffloat _h_scale_factor;

    int32_t mouse_x{};
    int32_t mouse_y{};

    void _init_sdl();
    void _init_window();
    void _init_renderer();

    void _load_sprites();
};
} // namespace sim
#endif // FENETREPRINCIPALE_H
