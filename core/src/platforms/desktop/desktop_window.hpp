#pragma once

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "../../base_types.hpp"
#include "desktop_app.hpp"

namespace aardvark {

class DesktopApp;

class DesktopWindow {
  public:
    DesktopWindow(DesktopApp* app, const Size& size);
    ~DesktopWindow();
    // Disable copy and assignment
    DesktopWindow(const DesktopWindow&) = delete;
    DesktopWindow& operator=(DesktopWindow const&) = delete;
    Size size;
    void make_current();
    void swap_now();
    void swap();
    DesktopApp* app;
    GLFWwindow* window;

    static DesktopWindow* get(GLFWwindow* window) {
        return static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));
    }
};

}  // namespace aardvark
