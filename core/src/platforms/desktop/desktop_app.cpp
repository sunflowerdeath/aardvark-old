#include "desktop_app.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include "../../utils/log.hpp"

namespace aardvark {

// Window events
void window_focus_callback(GLFWwindow* window, int focused) {
    auto event = focused ? static_cast<Event>(WindowFocusEvent())
                         : static_cast<Event>(WindowBlurEvent());
    DesktopApp::dispatch_event(window, event);
};

void window_close_callback(GLFWwindow* window) {
    DesktopApp::dispatch_event(window, WindowCloseEvent());
};

// Mouse events
void window_cursor_enter_callback(GLFWwindow* window, int entered) {
    auto event = entered ? static_cast<Event>(WindowCursorEnterEvent())
                         : static_cast<Event>(WindowCursorLeaveEvent());
    DesktopApp::dispatch_event(window, event);
};

int now() {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    return ms.count();
}

void cursor_pos_callback(GLFWwindow* window, double left, double top) {
    auto event = PointerEvent{
        now(),                        // timestamp
        PointerTool::mouse,           // tool
        0,                            // pointer_id
        PointerAction::pointer_move,  // action
        static_cast<float>(left),     // left
        static_cast<float>(top),      // top
        -1                            // button
    };
    DesktopApp::dispatch_event(window, event);
};

void mouse_button_callback(GLFWwindow* window, int button, int action,
                           int mods) {
    double left, top;
    glfwGetCursorPos(window, &left, &top);
    auto event = PointerEvent{
        now(),               // timestamp
        PointerTool::mouse,  // tool
        0,                   // pointer_id
        action == GLFW_PRESS ? PointerAction::button_press
                             : PointerAction::button_release,  // action
        static_cast<float>(left),                              // left
        static_cast<float>(top),                               // top
        button                                                 // button
    };
    DesktopApp::dispatch_event(window, event);
};

// Keyboard events
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods) {
    KeyAction key_action;
    if (action == GLFW_PRESS) key_action = KeyAction::Press;
    if (action == GLFW_RELEASE) key_action = KeyAction::Release;
    if (action == GLFW_REPEAT) key_action = KeyAction::Repeat;
    DesktopApp::dispatch_event(window,
                               KeyEvent{key, scancode, key_action, mods});
};

const auto FRAME_TIME = 16000;

void DesktopApp::dispatch_event(GLFWwindow* glfw_window, Event event) {
    auto window = DesktopWindow::get(glfw_window);
    window->app->handle_event(window, event);
};

std::shared_ptr<DesktopWindow> DesktopApp::create_window(Size size) {
    auto window = std::make_shared<DesktopWindow>(this, size);
    windows.push_back(window);
    auto glfw_window = window->window;
    // Window events
    glfwSetWindowFocusCallback(glfw_window, window_focus_callback);
    glfwSetWindowCloseCallback(glfw_window, window_close_callback);
    glfwSetCursorEnterCallback(glfw_window, window_cursor_enter_callback);
    // Mouse events
    glfwSetCursorPosCallback(glfw_window, cursor_pos_callback);
    glfwSetMouseButtonCallback(glfw_window, mouse_button_callback);
    // Keyboard events
    glfwSetKeyCallback(glfw_window, key_callback);
    documents[window.get()] = std::make_shared<Document>();
    return window;
};

std::shared_ptr<Document> DesktopApp::get_document(
    std::shared_ptr<DesktopWindow> window) {
    return documents[window.get()];
};

void DesktopApp::destroy_window(std::shared_ptr<DesktopWindow> window) {
    documents.erase(documents.find(window.get()));
    windows.erase(std::find(windows.begin(), windows.end(), window));
};

void DesktopApp::stop() { should_stop = true; };

void DesktopApp::run(std::function<void(void)> update_callback) {
    should_stop = false;
    event_loop->post_callback(
        std::bind(&DesktopApp::render, this, update_callback));
};

void DesktopApp::render(std::function<void(void)> update_callback) {
    if (should_stop) return;

    update_callback();

    auto start = std::chrono::high_resolution_clock::now();
    glfwPollEvents();

    bool painted = false;
    for (auto& window : windows) {
        window->make_current();
        painted = painted || documents[window.get()]->paint();
        window->swap_now();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto time =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start)
            .count();
    if (painted) {
        Log::debug("[DesktopApp] frame time {}ms", time / 1000.0);
    }
    auto timeout = (time < FRAME_TIME) ? (FRAME_TIME - time) : 0;
    event_loop->set_timeout(
        std::bind(&DesktopApp::render, this, update_callback), timeout);
}

void DesktopApp::handle_event(DesktopWindow* window, Event event) {
    if (event_handler) event_handler(this, event);

    // TODO dispatch keyboard events
    if (auto window_event = std::get_if<WindowEvent>(&event)) {
        // window->handle_event(window_event);
    } else if (auto pointer_event = std::get_if<PointerEvent>(&event)) {
        auto document = documents[window];
        if (!document->is_initial_paint) {
            document->pointer_event_manager->handle_event(*pointer_event);
        }
    }
};

}  // namespace aardvark
