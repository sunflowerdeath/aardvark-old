#include <chrono>
#include <thread>
#include <iostream>
#include "desktop_app.hpp"

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
  DesktopApp::dispatch_event(window, MouseMoveEvent(left, top));
};

void mouse_button_callback(GLFWwindow* window, int button, int action,
                           int mods) {
  auto event = action == GLFW_PRESS
                   ? static_cast<Event>(MouseDownEvent{button, mods})
                   : static_cast<Event>(MouseUpEvent{button, mods});
  DesktopApp::dispatch_event(window, event);
};

// Keyboard events
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods) {
  KeyAction key_action;
  if (action == GLFW_PRESS) key_action = KeyAction::Press;
  if (action == GLFW_RELEASE) key_action = KeyAction::Release;
  if (action == GLFW_REPEAT) key_action = KeyAction::Repeat;
  DesktopApp::dispatch_event(window, KeyEvent{key, scancode, key_action, mods});
};

const auto FRAME_TIME = 16000;

void DesktopApp::dispatch_event(GLFWwindow* window, Event event) {
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
  documents[window] = Document();
  return window;
};

Document* DesktopApp::get_document(std::shared_ptr<DesktopWindow> window) {
  return &documents[window];
};

void DesktopApp::destroy_window(std::shared_ptr<DesktopWindow> window) {
  documents.erase(documents.find(window));
  windows.erase(std::find(windows.begin(), windows.end(), window));
};

void DesktopApp::stop() { should_stop = true; };

void DesktopApp::run() {
  should_stop = false;
  bool painted = false;
  while (!should_stop) {
    auto start = std::chrono::high_resolution_clock::now();
    glfwPollEvents();
    painted = false;
    for (auto& window : windows) {
      window->make_current();
      painted = painted || documents[window].paint();
      window->swap_now();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto time =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start)
            .count();
    if (painted) {
      std::cout << "frame time: " << (time / 1000.0) << "ms" << std::endl;
    }
    if (time < FRAME_TIME) {
      // framerate is too high, need to wait
      std::this_thread::sleep_for(std::chrono::microseconds(16000 - time));
    }
  }
};

void DesktopApp::handle_event(GLFWwindow* window, Event event) {
  if (event_handler) event_handler(this, event);
};

}  // namespace aardvark
