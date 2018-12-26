#include <iostream>
#include <stdio.h>
#include "../desktop_app.hpp"
#include "../base_types.hpp"
#include "../elements/elements.hpp"

struct AppState {
  std::shared_ptr<aardvark::elements::Center> align;
  std::shared_ptr<aardvark::elements::Layer> layer;
  std::shared_ptr<aardvark::elements::Background> background;
  aardvark::elements::TransformOptions transform;
};

// TODO abs and relative origin
void to_matrix(aardvark::elements::TransformOptions transform,
                   SkMatrix* matrix) {
  matrix->setRotate(transform.rotation, 50, 50);
  matrix->postTranslate(transform.translate.left, transform.translate.top);
  matrix->postScale(transform.scale.horiz, transform.scale.vert,
                    transform.translate.left + 50,
                    transform.translate.top + 50);
};

void handle_events(aardvark::DesktopApp* app, aardvark::Event event) {
  auto state = (AppState*)app->user_pointer;
  if (auto key = std::get_if<aardvark::KeyEvent>(&event)) {
    if (key->action == aardvark::KeyAction::Press ||
        key->action == aardvark::KeyAction::Repeat) {
      if (key->key == GLFW_KEY_LEFT) {
        state->transform.translate.left -= 2;
      } else if (key->key == GLFW_KEY_RIGHT) {
        state->transform.translate.left += 2;
      } else if (key->key == GLFW_KEY_UP) {
        state->transform.translate.top -= 2;
      } else if (key->key == GLFW_KEY_DOWN) {
        state->transform.translate.top += 2;
      } else if (key->key == GLFW_KEY_Q) {
        state->transform.rotation -= 2;
      } else if (key->key == GLFW_KEY_W) {
        state->transform.rotation += 2;
      } else if (key->key == GLFW_KEY_Z) {
        state->transform.scale.horiz += 0.1;
        state->transform.scale.vert += 0.1;
      } else if (key->key == GLFW_KEY_X) {
        state->transform.scale.horiz -= 0.1;
        state->transform.scale.vert -= 0.1;
      }
    }

    SkMatrix matrix;
    to_matrix(state->transform, &matrix);
    state->layer->transform = matrix;
    state->layer->document->change_element(state->layer.get());
  }
}

int main() {
    auto app = aardvark::DesktopApp();
    auto window = app.create_window(aardvark::Size{500, 500});

    auto document = app.get_document(window);

    auto state = AppState{};

    auto transform = aardvark::elements::TransformOptions{
        aardvark::Position{0, 0},  // translate
        {1.0, 1.0},                // scale
        45                         // rotate
    };
    SkMatrix matrix;
    to_matrix(transform, &matrix);

    auto background =
        std::make_shared<aardvark::elements::Background>(SK_ColorRED);
    auto start = [&state]() { state.background->set_props(SK_ColorBLUE); };
    auto end = [&state](bool is_terminated) {
        state.background->set_props(SK_ColorRED);
    };
    auto layer = std::make_shared<aardvark::elements::Layer>(
        std::make_shared<aardvark::elements::GestureResponder>(
            background,                             // child
            aardvark::ResponderMode::PassToParent,  // mode
            start,                                  // start
            [] {},                                  // update
            end),                                   // end
        matrix);
    auto align = std::make_shared<aardvark::elements::Center>(
        std::make_shared<aardvark::elements::FixedSize>(
            layer, aardvark::Size{100 /* width */, 100 /* height */}));

    document->set_root(align);

    state.align = align;
    state.layer = layer;
    state.background = background;
    state.transform = transform;

    app.user_pointer = (void*)(&state);
    app.event_handler = &handle_events;
    app.run();
};
