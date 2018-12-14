#include <iostream>
#include "../app.hpp"
#include "../base_types.hpp"
#include "../elements/elements.hpp"

void handle_events(aardvark::DesktopApp* app, aardvark::Event event) {
  if (std::holds_alternative<aardvark::WindowFocusEvent>(event)) {
    std::cout << "focus" << std::endl;
  } else if (std::holds_alternative<aardvark::WindowBlurEvent>(event)) {
    std::cout << "blur" << std::endl;
  }
}

int main() {
  auto app = aardvark::DesktopApp();
  auto window = app.create_window(aardvark::Size{500, 500});
  auto document = app.get_document(window);
  std::shared_ptr<aardvark::Element> background =
      std::make_shared<aardvark::elements::Background>(SK_ColorRED);
  document->set_root(background);
  app.event_handler = &handle_events;
  app.run();
}
