#include <chrono>
#include <thread>
#include <iostream>
#include "app.hpp"

namespace aardvark {

/*
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods) {}

void cursor_pos_callback(GLFWwindow* window, double left, double top) {
  App::dispatch_event(window, events::MouseMoveEvent(left, top));
}


void mouse_button_callback(GLFWwindow* window, int button, int action,
                           int mods) {
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    popup_menu();
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
}

void drop_callback(GLFWwindow* window, int count, const char** paths) {
  int i;
  for (i = 0;  i < count;  i++)
      handle_dropped_file(paths[i]);
}

void window_size_callback(GLFWwindow* window, int width, int height) {
  App::dispatch_event(events::ResizeEvent(width, height));
}
*/

void window_focus_callback(GLFWwindow* window, int focused) {
  if (focused) DesktopApp::dispatch_event(window, WindowFocusEvent());
  else DesktopApp::dispatch_event(window, WindowBlurEvent());
};

void cursor_enter_callback(GLFWwindow* window, int entered) {
  if (entered) DesktopApp::dispatch_event(window, MouseEnterEvent());
  else DesktopApp::dispatch_event(window, MouseLeaveEvent());
}

void cursor_pos_callback(GLFWwindow* window, double left, double top) {
  DesktopApp::dispatch_event(window, MouseMoveEvent(left, top));
}

const auto FRAME_TIME = 16000;

void DesktopApp::dispatch_event(GLFWwindow* window, Event event) {
  auto inst = (DesktopApp*)glfwGetWindowUserPointer(window);
  inst->handle_event(window, event);
};

std::shared_ptr<DesktopWindow> DesktopApp::create_window(Size size) {
  auto window = std::make_shared<DesktopWindow>(size);
  windows.push_back(window);
  glfwSetWindowUserPointer(window->window, (void*)this);
  glfwSetWindowFocusCallback(window->window, window_focus_callback);
  glfwSetCursorEnterCallback(window->window, cursor_enter_callback);
  glfwSetCursorPosCallback(window->window, cursor_pos_callback);
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
