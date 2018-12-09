#include "window.hpp"

namespace aardvark {

const int STENCIL_BITS = 8;
const int MSAA_SAMPLE_COUNT = 4;  // 4;

GlfwWindow::GlfwWindow(Size size) {
  if (!glfwInit()) {
    std::cout << "glfw init error" << std::endl;
  }
  glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
  glfwWindowHint(GLFW_RED_BITS, 8);
  glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8);
  glfwWindowHint(GLFW_DEPTH_BITS, 0);
  glfwWindowHint(GLFW_STENCIL_BITS, STENCIL_BITS);
  glfwWindowHint(GLFW_SAMPLES, MSAA_SAMPLE_COUNT);
  window = glfwCreateWindow(size.width, size.height, "GLFW", NULL, NULL);
  make_current();
};

GlfwWindow::~GlfwWindow() { glfwDestroyWindow(window); };
void GlfwWindow::swap() { glfwSwapBuffers(window); };
void GlfwWindow::swap_now() { glfwSwapBuffers(window); };
void GlfwWindow::make_current() { glfwMakeContextCurrent(window); };

} // namespace aardvark
