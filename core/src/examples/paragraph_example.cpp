#include <iostream>
#include <stdio.h>
#include <unicode/unistr.h>
#include "SkPaint.h"
#include "../desktop_app.hpp"
#include "../base_types.hpp"
#include "../elements/elements.hpp"
#include "../inline_layout/inline_layout.hpp"
#include "../inline_layout/text_span.hpp"

struct AppState {
  std::shared_ptr<aardvark::elements::FixedSize> fixed_size;
};

void handle_events(aardvark::DesktopApp* app, aardvark::Event event) {
  auto state = (AppState*)app->user_pointer;
  if (auto key = std::get_if<aardvark::KeyEvent>(&event)) {
    if (key->action == aardvark::KeyAction::Press ||
        key->action == aardvark::KeyAction::Repeat) {
      if (key->key == GLFW_KEY_LEFT) {
        state->fixed_size->size.width -= 2;
      } else if (key->key == GLFW_KEY_RIGHT) {
        state->fixed_size->size.width += 2;
      }
    }

    state->fixed_size->document->change_element(state->fixed_size.get());
  }
}

int main() {
    auto app = aardvark::DesktopApp();
    auto window = app.create_window(aardvark::Size{500, 500});
    auto document = app.get_document(window);

    auto text =
        UnicodeString((UChar*)u"Lorem ipsum dolor sit amet, consectetur");
    SkPaint paint;
    paint.setColor(SK_ColorWHITE);

    auto fixed_size = std::make_shared<aardvark::elements::FixedSize>(
        std::make_shared<aardvark::elements::Border>(
            std::make_shared<aardvark::elements::Paragraph>(
                std::vector<std::shared_ptr<aardvark::inline_layout::Span>>{
                    std::make_shared<aardvark::inline_layout::TextSpan>(
                        text, paint)}),
            aardvark::elements::BoxBorders::all(
                aardvark::elements::BorderSide{/* width */ 1,
                                               /* color */ SK_ColorWHITE}),
            aardvark::elements::BoxRadiuses::all(
                aardvark::elements::Radius::circular(0))),
        aardvark::Size{100 /* width */, 120 /* height */});
    auto elem = std::make_shared<aardvark::elements::Align>(
        fixed_size, aardvark::value::abs(50), aardvark::value::abs(50));
    document->set_root(elem);
    auto state = AppState{fixed_size};
    app.user_pointer = (void*)(&state);
    app.event_handler = &handle_events;
    app.run();
};
