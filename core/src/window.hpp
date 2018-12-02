#pragma once

#include <iostream>
#include <GL/gl.h>
#include <SDL.h>
// #include <GLFW/glfw3.h>
#include "base_types.hpp"

namespace aardvark {

class SdlWindow {
 public:
  SdlWindow(Size size);
  ~SdlWindow();
  void make_current();
  void swap_now();
  void swap();
  void close();
  SDL_Window* window;
  SDL_GLContext context;
};

}  // namespace aardvark
