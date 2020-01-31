#include "platforms/desktop/desktop_window.hpp"
#include "utils/log.hpp"

namespace aardvark {

const int STENCIL_BITS = 8;
const int MSAA_SAMPLE_COUNT = 0;

DesktopWindow::DesktopWindow(DesktopApp* app, const Size& size)
    : app(app), size(size) {
    if (!glfwInit()) {
        Log::error("[DesktopWindow] glfw init error");
    }
    glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 0);
    glfwWindowHint(GLFW_STENCIL_BITS, STENCIL_BITS);
    glfwWindowHint(GLFW_SAMPLES, MSAA_SAMPLE_COUNT);
    window = glfwCreateWindow(size.width, size.height, "GLFW", NULL, NULL);
    if (window == nullptr) {
        Log::error("[DesktopWindow] Cannot create GLFW window");
    }
    glfwSetWindowUserPointer(window, static_cast<void*>(this));
    make_current();
    glfwSwapInterval(0);
};

DesktopWindow::~DesktopWindow() { glfwDestroyWindow(window); };

void DesktopWindow::swap() { glfwSwapBuffers(window); };

void DesktopWindow::swap_now() { glfwSwapBuffers(window); };

void DesktopWindow::make_current() { glfwMakeContextCurrent(window); };

void DesktopWindow::set_size(const Size& size) {
    this->size = size;
    glfwSetWindowSize(window, size.width, size.height);
}

void DesktopWindow::set_position(const Position& pos) {
    glfwSetWindowPos(window, pos.left, pos.top);
}

void DesktopWindow::set_title(const char* title) {
    glfwSetWindowTitle(window, title);
}

void DesktopWindow::minimize() { glfwIconifyWindow(window); }

void DesktopWindow::restore() { glfwRestoreWindow(window); }

void DesktopWindow::maximize() { glfwMaximizeWindow(window); }

void DesktopWindow::hide() { glfwHideWindow(window); }

void DesktopWindow::focus() { glfwFocusWindow(window); }

}  // namespace aardvark
