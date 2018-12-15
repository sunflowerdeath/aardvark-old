#pragma once

#include <iostream>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "base_types.hpp"

namespace aardvark {

class DesktopWindow {
 public:
  DesktopWindow(Size size);
  ~DesktopWindow();
  // Disable copy and assignment
  DesktopWindow(const DesktopWindow&) = delete;
  DesktopWindow& operator=(DesktopWindow const&) = delete;
  void make_current();
  void swap_now();
  void swap();
  GLFWwindow* window;
};

}  // namespace aardvark
