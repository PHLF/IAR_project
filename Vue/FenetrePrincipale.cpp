#include "FenetrePrincipale.h"

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
    _init_renderer();
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
    SDL_SetRenderDrawColor(_renderer.get(), 255, 0, 0, 255);
  }
}
