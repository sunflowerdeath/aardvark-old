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

    void make_current();
    void swap_now();
    void swap();

    void set_size(const Size& size);
    void set_position(const Position& pos);
    void set_title(const char* title);
    void minimize();
    void maximize();
    void restore();
    void hide();
    void focus();

    Size size;
    DesktopApp* app;
    GLFWwindow* window;

    static DesktopWindow* get(GLFWwindow* window) {
        return static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));
    }
};

}  // namespace aardvark
