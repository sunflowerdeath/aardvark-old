#pragma once

// #include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "../../events.hpp"
#include "../../base_types.hpp"
#include "../../pointer_events/signal_event_sink.hpp"
#include "desktop_app.hpp"

namespace aardvark {

class DesktopApp;

struct DesktopWindowOptions {
    std::string title = "";
    std::optional<Position> position = std::nullopt;
    Size size;
    // std::optional<Size> min_size = std::nullopt;    
    // std::optional<Size> max_size = std::nullopt;
    bool visible = true;
    bool decorated = true;
    bool resizable = true;
    bool floating = false;
    bool maximized = false;
};

class DesktopWindow {
  public:
    DesktopWindow(DesktopApp* app, const DesktopWindowOptions& options);
    DesktopWindow(DesktopApp* app, const Size& size);
    ~DesktopWindow();
    // Disable copy and assignment
    DesktopWindow(const DesktopWindow&) = delete;
    DesktopWindow& operator=(DesktopWindow const&) = delete;

    void make_current();
    void swap_now();
    void swap();

    Size get_size();
    void set_size(const Size& size);
    
    Position get_position();
    void set_position(const Position& pos);
    
    std::string get_title();
    void set_title(const std::string& title);
    
    void minimize();
    void maximize();
    void restore();
    void hide();
    void show();
    void focus();

    SignalEventSink<WindowEvent> window_event_sink;
    SignalEventSink<KeyEvent> key_event_sink;
    SignalEventSink<CharEvent> char_event_sink;
    SignalEventSink<ScrollEvent> scroll_event_sink;
    SignalEventSink<PointerEvent> pointer_event_sink;

    std::shared_ptr<Connection> add_pointer_event_handler(
        const SignalEventSink<PointerEvent>::EventHandler& handler);
    std::shared_ptr<Connection> add_key_event_handler(
        const SignalEventSink<KeyEvent>::EventHandler& handler);
    std::shared_ptr<Connection> add_char_event_handler(
        const SignalEventSink<CharEvent>::EventHandler& handler);
    std::shared_ptr<Connection> add_scroll_event_handler(
        const SignalEventSink<ScrollEvent>::EventHandler& handler);
    std::shared_ptr<Connection> add_window_event_handler(
        const SignalEventSink<WindowEvent>::EventHandler& handler);

    DesktopApp* app;
    GLFWwindow* window;

    static DesktopWindow* get(GLFWwindow* window) {
        return static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));
    }
    
  private:
    std::string title = "";

    void create_with_options(const DesktopWindowOptions& options);
};

}  // namespace aardvark
