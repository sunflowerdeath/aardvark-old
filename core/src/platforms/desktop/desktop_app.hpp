#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>

#include <GLFW/glfw3.h>

#include "../../base_types.hpp"
#include "../../events.hpp"
#include "../../document.hpp"
#include "../../utils/event_loop.hpp"
#include "desktop_window.hpp"

namespace aardvark {

class DesktopApp {
  public:
    DesktopApp(std::shared_ptr<EventLoop> event_loop)
        : event_loop(event_loop){};

    // Runs application loop - polls events, calls handlers and repaints
    void run(std::function<void(void)> update_callback = nullptr);

    void render(std::function<void(void)> update_callback);

    // Stops application loop
    void stop();

    // Creates new window
    std::shared_ptr<DesktopWindow> create_window(Size size);

    void destroy_window(std::shared_ptr<DesktopWindow> window);

    std::shared_ptr<Document> get_document(
        std::shared_ptr<DesktopWindow> window);

    // User provided event handler
    std::function<void(DesktopApp* app, Event event)> event_handler;
 
    // Pointer to user data, for example, an application state
    void* user_pointer;

    std::vector<std::shared_ptr<DesktopWindow>> windows;

    // Dispatches event to the corresponding App instance
    static void dispatch_event(GLFWwindow* window, Event event);

  private:
    std::shared_ptr<EventLoop> event_loop;
    bool should_stop;
    std::unordered_map<std::shared_ptr<DesktopWindow>,
                       std::shared_ptr<Document>>
        documents;
    void handle_event(GLFWwindow* window, Event event);
};

}  // namespace aardvark
