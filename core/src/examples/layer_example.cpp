#include <iostream>
#include <stdio.h>
#include "../desktop_app.hpp"
#include "../base_types.hpp"
#include "../elements/elements.hpp"

struct AppState {
  std::shared_ptr<aardvark::elements::Center> align;
  std::shared_ptr<aardvark::elements::Layer> layer;
};

void handle_events(aardvark::DesktopApp* app, aardvark::Event event) {
  auto state = (AppState*)app->user_pointer;
  if (auto key = std::get_if<aardvark::KeyEvent>(&event)) {
    auto type = key->action == aardvark::KeyAction::Press
                   ? "press"
                   : (key->action == aardvark::KeyAction::Release ? "release"
                                                                  : "repeat");
    auto name = glfwGetKeyName(key->key, key->scancode);
    std::cout << "key " << type << " " << (name == NULL ? "?" : name)
              << std::endl;
  }
}

int main() {
  auto app = aardvark::DesktopApp();
  auto window = app.create_window(aardvark::Size{500, 500});

  auto document = app.get_document(window);

  auto transform = aardvark::elements::TransformOptions{
      aardvark::Position{0, 0},  // translate
      1.0,                       // scale
      45                         // rotate
  };

  SkMatrix matrix;
  matrix.setRotate(45, 50, 50);
  matrix.postTranslate(100, 100);

  auto layer = std::make_shared<aardvark::elements::Layer>(
      std::make_shared<aardvark::elements::Background>(SK_ColorRED), matrix);
  auto elem = std::make_shared<aardvark::elements::Center>(
      std::make_shared<aardvark::elements::FixedSize>(
          layer, aardvark::Size{100 /* width */, 100 /* height */}));
  document->set_root(elem);
  auto state = AppState{elem, layer};
  //app.user_pointer = (void*)(&state);
  // app.event_handler = &handle_events;
  app.run();
};
