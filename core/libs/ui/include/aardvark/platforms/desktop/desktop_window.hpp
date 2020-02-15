#pragma once

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "../../events.hpp"
#include "../../base_types.hpp"
#include "../../pointer_events/signal_event_sink.hpp"
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

    SignalEventSink<WindowEvent> window_event_sink;
    SignalEventSink<KeyEvent> key_event_sink;
    SignalEventSink<ScrollEvent> scroll_event_sink;
    SignalEventSink<PointerEvent> pointer_event_sink;

    std::shared_ptr<Connection> add_pointer_event_handler(
        const SignalEventSink<PointerEvent>::EventHandler& handler);
    std::shared_ptr<Connection> add_key_event_handler(
        const SignalEventSink<KeyEvent>::EventHandler& handler);
    std::shared_ptr<Connection> add_scroll_event_handler(
        const SignalEventSink<ScrollEvent>::EventHandler& handler);

    Size size;
    DesktopApp* app;
    GLFWwindow* window;

    static DesktopWindow* get(GLFWwindow* window) {
        return static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));
    }
};

}  // namespace aardvark
