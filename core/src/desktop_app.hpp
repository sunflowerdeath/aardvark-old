#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include <GLFW/glfw3.h>
#include "base_types.hpp"
#include "desktop_window.hpp"
#include "events.hpp"
#include "document.hpp"

namespace aardvark {

class DesktopApp {
 public:
  // Runs application loop - polls events, calls handlers and repaints
  void run();
  // Stops application loop
  void stop();
  std::shared_ptr<DesktopWindow> create_window(Size size);
  void destroy_window(std::shared_ptr<DesktopWindow> window);
  Document* get_document(std::shared_ptr<DesktopWindow> window);
  // Used provided event handler
  std::function<void(DesktopApp* app, Event event)> event_handler;
  // Pointer to user data, for example
  void* user_pointer;
  std::optional<MouseMoveEvent> most_recent_mousemove;
  // Dispatches event to the corresponding App instance
  static void dispatch_event(GLFWwindow* window, Event event);
 private:
  bool should_stop;
  std::vector<std::shared_ptr<DesktopWindow>> windows;
  std::unordered_map<std::shared_ptr<DesktopWindow>, Document> documents;
  void handle_event(GLFWwindow* window, Event event);
};

}  // namespace aardvark
