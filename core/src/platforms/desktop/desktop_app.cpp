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
void cursor_enter_callback(GLFWwindow* window, int entered) {
    auto event = entered ? static_cast<Event>(MouseEnterEvent())
                         : static_cast<Event>(MouseLeaveEvent());
    DesktopApp::dispatch_event(window, event);
};

void cursor_pos_callback(GLFWwindow* window, double left, double top) {
    auto event = PointerEvent{
        PointerTool::mouse,           // tool
        0,                            // pointer_id
        PointerAction::pointer_move,  // action
        static_cast<float>(left),     // left
        static_cast<float>(top)       // top
    };
    DesktopApp::dispatch_event(window, event);
};

void mouse_button_callback(GLFWwindow* window, int button, int action,
                           int mods) {
    double left, top;
    glfwGetCursorPos(window, &left, &top);
    auto event = ButtonEvent{
        PointerTool::mouse,  // tool
        0,                   // pointer_id
        action == GLFW_PRESS ? PointerAction::button_press
                             : PointerAction::button_release,  // action
        static_cast<float>(left),                              // left
        static_cast<float>(top),                               // top
        button                                                 // button_id
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

void DesktopApp::dispatch_event(GLFWwindow* window, Event event) {
    // event.timestamp = now();
    auto inst = (DesktopApp*)glfwGetWindowUserPointer(window);
    inst->handle_event(window, event);
};

std::shared_ptr<DesktopWindow> DesktopApp::create_window(Size size) {
    auto window = std::make_shared<DesktopWindow>(size);
    windows.push_back(window);
    glfwSetWindowUserPointer(window->window, (void*)this);
    // Window events
    glfwSetWindowFocusCallback(window->window, window_focus_callback);
    glfwSetWindowCloseCallback(window->window, window_close_callback);
    // Mouse events
    glfwSetCursorEnterCallback(window->window, cursor_enter_callback);
    glfwSetCursorPosCallback(window->window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window->window, mouse_button_callback);
    // Keyboard events
    glfwSetKeyCallback(window->window, key_callback);
    documents[window] = std::make_shared<Document>();
    return window;
};

std::shared_ptr<Document> DesktopApp::get_document(
    std::shared_ptr<DesktopWindow> window) {
    return documents[window];
};

void DesktopApp::destroy_window(std::shared_ptr<DesktopWindow> window) {
    documents.erase(documents.find(window));
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

    auto start = std::chrono::high_resolution_clock::now();
    glfwPollEvents();

    bool painted = false;
    for (auto& window : windows) {
        window->make_current();
        painted = painted || documents[window]->paint();
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

void DesktopApp::handle_event(GLFWwindow* window, Event event) {
    if (event_handler) event_handler(this, event);
    // dispatch window events (close/minimize/maximize)
    if (auto pointer_event = std::get_if<PointerEvent>(&event)) {
        auto win = windows[0];  // TODO: support multiple windows
        auto doc = get_document(win);
        if (!doc->is_initial_paint) {
            doc->pointer_event_manager->handle_event(*pointer_event);
        }
    }
};

}  // namespace aardvark
