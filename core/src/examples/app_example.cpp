#include <iostream>
#include "../app.hpp"
#include "../base_types.hpp"
#include "../elements/elements.hpp"

struct AppState {
  std::shared_ptr<aardvark::elements::Background> background;
};

void handle_events(aardvark::DesktopApp* app, aardvark::Event event) {
  auto state = (AppState*)app->user_pointer;
  if (std::holds_alternative<aardvark::WindowFocusEvent>(event)) {
    std::cout << "window focus" << std::endl;
  } else if (std::holds_alternative<aardvark::WindowBlurEvent>(event)) {
    std::cout << "window blur" << std::endl;
  } else if (std::holds_alternative<aardvark::MouseEnterEvent>(event)) {
    std::cout << "mouse enter" << std::endl;
    state->background->set_props(SK_ColorRED);
  } else if (std::holds_alternative<aardvark::MouseLeaveEvent>(event)) {
    std::cout << "mouse leave" << std::endl;
    state->background->set_props(SK_ColorBLUE);
  } else if (auto mousemove = std::get_if<aardvark::MouseMoveEvent>(&event)) {
    std::cout << "mouse move " << mousemove->left << "," << mousemove->top
              << std::endl;
  }
};

int main() {
  auto app = aardvark::DesktopApp();
  auto window = app.create_window(aardvark::Size{500, 500});
  auto document = app.get_document(window);
  auto background =
      std::make_shared<aardvark::elements::Background>(SK_ColorRED);
  document->set_root(background);
  auto state = AppState{background};
  app.user_pointer = (void*)(&state);
  app.event_handler = &handle_events;
  app.run();
};
