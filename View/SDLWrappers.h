#ifndef SDLWRAPPERS_H
#define SDLWRAPPERS_H

#include <Misc/Globals.h>
#include <SDL2/SDL.h>

namespace sim {
struct SDL_Deleter {
  void operator()(SDL_Surface* ptr) {
    if (ptr != nullptr) {
      SDL_FreeSurface(ptr);
    }
  }
  void operator()(SDL_Texture* ptr) {
    if (ptr != nullptr) {
      SDL_DestroyTexture(ptr);
    }
  }
  void operator()(SDL_Renderer* ptr) {
    if (ptr != nullptr) {
      SDL_DestroyRenderer(ptr);
    }
  }
  void operator()(SDL_Window* ptr) {
    if (ptr != nullptr) {
      SDL_DestroyWindow(ptr);
    }
  }
  void operator()(SDL_RWops* ptr) {
    if (ptr != nullptr) {
      SDL_RWclose(ptr);
    }
  }
};

// Unique Pointers
using SurfacePtr = std::unique_ptr<SDL_Surface, SDL_Deleter>;
using TexturePtr = std::unique_ptr<SDL_Texture, SDL_Deleter>;
using RendererPtr = std::unique_ptr<SDL_Renderer, SDL_Deleter>;
using WindowPtr = std::unique_ptr<SDL_Window, SDL_Deleter>;
using RWopsPtr = std::unique_ptr<SDL_RWops, SDL_Deleter>;
}

#endif  // SDLWRAPPERS_H
