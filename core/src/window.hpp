#pragma once

#include <iostream>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "base_types.hpp"

namespace aardvark {

class GlfwWindow {
 public:
  GlfwWindow(Size size);
  ~GlfwWindow();
  void make_current();
  void swap_now();
  void swap();
  GLFWwindow* window;
};

}  // namespace aardvark
