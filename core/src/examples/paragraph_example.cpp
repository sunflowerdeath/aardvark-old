#include <iostream>
#include <unicode/unistr.h>
#include "SkPaint.h"
#include "../platforms/desktop/desktop_app.hpp"
#include "../base_types.hpp"
#include "../elements/elements.hpp"
#include "../inline_layout/text_span.hpp"
#include "../inline_layout/decoration_span.hpp"

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
    paint.setTextSize(16);
    auto span =
        std::make_shared<aardvark::inline_layout::TextSpan>(text, paint);

    auto red_text = UnicodeString((UChar*)u"MORE TEXT");
    SkPaint red_paint;
    red_paint.setColor(SK_ColorRED);
    red_paint.setTextSize(32);
    auto red_span = std::make_shared<aardvark::inline_layout::TextSpan>(
        red_text, red_paint);

    auto green_text =
        UnicodeString((UChar*)u" reference to the last element in the vector");
    SkPaint green_paint;
    green_paint.setColor(SK_ColorGREEN);
    green_paint.setTextSize(16);
    auto green_span = std::make_shared<aardvark::inline_layout::TextSpan>(
        green_text, green_paint);

    auto decoration = aardvark::inline_layout::Decoration{
        SK_ColorGRAY  // background
    };
    auto decoration_span =
        std::make_shared<aardvark::inline_layout::DecorationSpan>(
            std::vector<std::shared_ptr<aardvark::inline_layout::Span>>{
                red_span, green_span},
            decoration);

    auto paragraph = std::make_shared<aardvark::elements::Paragraph>(
        std::vector<std::shared_ptr<aardvark::inline_layout::Span>>{
            span,
            // red_span, green_span},
            decoration_span},
        aardvark::inline_layout::LineMetrics::from_paint(paint));

    auto fixed_size = std::make_shared<aardvark::elements::FixedSize>(
        std::make_shared<aardvark::elements::Border>(
            paragraph,
            aardvark::elements::BoxBorders::all(
                aardvark::elements::BorderSide{/* width */ 1,
                                               /* color */ SK_ColorWHITE}),
            aardvark::elements::BoxRadiuses::all(
                aardvark::elements::Radius::circular(0))),
        aardvark::Size{200 /* width */, 150 /* height */});
    auto elem = std::make_shared<aardvark::elements::Align>(
        fixed_size, aardvark::elements::EdgeInsets{aardvark::Value::abs(50),
                                                   aardvark::Value::abs(50)});
    document->set_root(elem);
    auto state = AppState{fixed_size};
    app.user_pointer = (void*)(&state);
    app.event_handler = &handle_events;
    app.run();
};
