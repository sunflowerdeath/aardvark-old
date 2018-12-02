#include "window.hpp"

namespace aardvark {

void handle_sdl_error() {
  const char* error = SDL_GetError();
  std::cout << "SDL Error: " << error;
  SDL_ClearError();
}

SdlWindow::SdlWindow(Size size) {
  // Skia needs 8 stencil bits but it works with 0
  const int stencil_bits = 8;
  const int msaa_sample_count = 0;  // 4;

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, stencil_bits);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

  // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, msaa_sample_count);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
    handle_sdl_error();
  }

  window = SDL_CreateWindow(
      "SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.height,
      size.width, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    handle_sdl_error();
  }

  context = SDL_GL_CreateContext(window);
  if (!context) {
    handle_sdl_error();
  }
};

SdlWindow::~SdlWindow() {
  SDL_Quit();
}

void SdlWindow::close() {
  std::cout << "close window" << std::endl;
  SDL_DestroyWindow(window);
};


void SdlWindow::make_current() {
  int result = SDL_GL_MakeCurrent(window, context);
  if (result != 0) {
    handle_sdl_error();
  }
}

void SdlWindow::swap_now() {
  SDL_GL_SwapWindow(window);
}

void SdlWindow::swap() {
  SDL_GL_SetSwapInterval(1);
  SDL_GL_SwapWindow(window);
}

} // namespace aardvark
