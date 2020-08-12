#include "platforms/desktop/desktop_window.hpp"
#include "utils/log.hpp"

namespace aardvark {

const int STENCIL_BITS = 8;
const int MSAA_SAMPLE_COUNT = 0;

DesktopWindow::DesktopWindow(DesktopApp* app, const Size& size) : app(app) {
    auto options = DesktopWindowOptions();
    options.size = size;
    create_with_options(options);
};

DesktopWindow::DesktopWindow(
    DesktopApp* app, const DesktopWindowOptions& options)
    : app(app) {
    create_with_options(options);
}

void DesktopWindow::create_with_options(const DesktopWindowOptions& options) {
    if (!glfwInit()) {
        Log::error("[DesktopWindow] glfw init error");
    }

    glfwWindowHint(GLFW_VISIBLE, options.visible);
    glfwWindowHint(GLFW_RESIZABLE, options.resizable);
    glfwWindowHint(GLFW_DECORATED, options.decorated);
    glfwWindowHint(GLFW_FLOATING, options.floating);
    glfwWindowHint(GLFW_MAXIMIZED, options.maximized);

    // OpenGL flags
    glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 0);
    glfwWindowHint(GLFW_STENCIL_BITS, STENCIL_BITS);
    glfwWindowHint(GLFW_SAMPLES, MSAA_SAMPLE_COUNT);

    window = glfwCreateWindow(
        (int)options.size.width,
        (int)options.size.height,
        options.title.data(),
        nullptr,
        nullptr);
    if (window == nullptr) {
        Log::error("[DesktopWindow] Cannot create GLFW window");
    }
    if (options.position.has_value()) set_position(options.position.value());
    glfwSetWindowUserPointer(window, static_cast<void*>(this));
    make_current();
    glfwSwapInterval(0);
}

DesktopWindow::~DesktopWindow() { glfwDestroyWindow(window); };

void DesktopWindow::swap() { glfwSwapBuffers(window); };

void DesktopWindow::swap_now() { glfwSwapBuffers(window); };

void DesktopWindow::make_current() { glfwMakeContextCurrent(window); };

Size DesktopWindow::get_size() {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    return Size{static_cast<float>(width), static_cast<float>(height)};
}

void DesktopWindow::set_size(const Size& size) {
    glfwSetWindowSize(window, (int)size.width, (int)size.height);
}

Position DesktopWindow::get_position() {
    int left = 0;
    int top = 0;
    glfwGetWindowPos(window, &left, &top);
    return Position((float)left, (float)top);
}

void DesktopWindow::set_position(const Position& pos) {
    glfwSetWindowPos(window, (int)pos.left, (int)pos.top);
}

void DesktopWindow::set_title(const char* title) {
    glfwSetWindowTitle(window, title);
}

void DesktopWindow::minimize() { glfwIconifyWindow(window); }

void DesktopWindow::restore() { glfwRestoreWindow(window); }

void DesktopWindow::maximize() { glfwMaximizeWindow(window); }

void DesktopWindow::hide() { glfwHideWindow(window); }

void DesktopWindow::focus() { glfwFocusWindow(window); }

std::shared_ptr<Connection> DesktopWindow::add_pointer_event_handler(
    const SignalEventSink<PointerEvent>::EventHandler& handler) {
    return pointer_event_sink.add_handler(handler);
}

std::shared_ptr<Connection> DesktopWindow::add_key_event_handler(
    const SignalEventSink<KeyEvent>::EventHandler& handler){
    return key_event_sink.add_handler(handler);
}

std::shared_ptr<Connection> DesktopWindow::add_char_event_handler(
    const SignalEventSink<CharEvent>::EventHandler& handler){
    return char_event_sink.add_handler(handler);
}

std::shared_ptr<Connection> DesktopWindow::add_scroll_event_handler(
    const SignalEventSink<ScrollEvent>::EventHandler& handler) {
    return scroll_event_sink.add_handler(handler);
}

std::shared_ptr<Connection> DesktopWindow::add_window_event_handler(
    const SignalEventSink<WindowEvent>::EventHandler& handler) {
    return window_event_sink.add_handler(handler);
}

}  // namespace aardvark
