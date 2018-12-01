#pragma once

#include <iostream>
#include <GL/gl.h>
#include <SDL.h>
#include "base_types.hpp"

namespace aardvark {

class SdlWindow {
 public:
  SdlWindow(Size size);
  ~SdlWindow();
  void handleEvents();
  void make_current();
  void swap_now();
  void swap();
  void close();
  GLint fboid;
  SDL_Window* window;
  SDL_GLContext glContext;
};

}  // namespace aardvark
