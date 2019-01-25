#include <iostream>
#include <stdio.h>
#include "../desktop_app.hpp"
#include "../base_types.hpp"
#include "../elements/elements.hpp"

struct AppState {
  std::shared_ptr<aardvark::elements::Align> align;
  std::shared_ptr<aardvark::elements::Background> background;
};

void handle_events(aardvark::DesktopApp* app, aardvark::Event event) {
  auto state = (AppState*)app->user_pointer;
  if (std::holds_alternative<aardvark::WindowFocusEvent>(event)) {
    std::cout << "window focus" << std::endl;
  } else if (std::holds_alternative<aardvark::WindowCloseEvent>(event)) {
    std::cout << "window close" << std::endl;
    app->stop();
  } else if (std::holds_alternative<aardvark::WindowBlurEvent>(event)) {
    std::cout << "window blur" << std::endl;
  } else if (std::holds_alternative<aardvark::MouseEnterEvent>(event)) {
    std::cout << "mouse enter" << std::endl;
  } else if (std::holds_alternative<aardvark::MouseLeaveEvent>(event)) {
    std::cout << "mouse leave" << std::endl;
  } else if (std::holds_alternative<aardvark::MouseDownEvent>(event)) {
    std::cout << "mouse down" << std::endl;
  } else if (std::holds_alternative<aardvark::MouseUpEvent>(event)) {
    std::cout << "mouse up" << std::endl;
  } else if (auto mousemove = std::get_if<aardvark::MouseMoveEvent>(&event)) {
      state->align->insets = aardvark::elements::EdgeInsets{
          aardvark::Value::abs(mousemove->left - 20),  // left
          aardvark::Value::abs(mousemove->top - 20)    // top
      };
      state->align->document->change_element(state->align.get());
  } else if (auto key = std::get_if<aardvark::KeyEvent>(&event)) {
    auto type = key->action == aardvark::KeyAction::Press
                   ? "press"
                   : (key->action == aardvark::KeyAction::Release ? "release"
                                                                  : "repeat");
    auto name = glfwGetKeyName(key->key, key->scancode);
    std::cout << "key " << type << " " << (name == NULL ? "?" : name)
              << std::endl;
  }
};

int main() {
  auto app = aardvark::DesktopApp();
  auto window = app.create_window(aardvark::Size{500, 500});
  auto document = app.get_document(window);

  auto background =
      std::make_shared<aardvark::elements::Background>(SK_ColorRED);
  auto elem = std::make_shared<aardvark::elements::Align>(
      std::make_shared<aardvark::elements::FixedSize>(
          background, aardvark::Size{40 /* width */, 40 /* height */}),
      aardvark::elements::EdgeInsets{aardvark::Value::abs(0),
                                     aardvark::Value::abs(0)});
  document->set_root(elem);
  auto state = AppState{elem, background};
  app.user_pointer = (void*)(&state);
  app.event_handler = &handle_events;
  app.run();
};
