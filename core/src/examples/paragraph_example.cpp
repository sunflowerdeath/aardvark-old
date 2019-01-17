#include <iostream>
#include <stdio.h>
#include <unicode/unistr.h>
#include "SkPaint.h"
#include "../desktop_app.hpp"
#include "../base_types.hpp"
#include "../elements/elements.hpp"
#include "../inline_layout/inline_layout.hpp"
#include "../inline_layout/text_span.hpp"

/* struct AppState { */
  /* std::shared_ptr<aardvark::elements::Align> align; */
  /* std::shared_ptr<aardvark::elements::Background> background; */
/* }; */

int main() {
    auto app = aardvark::DesktopApp();
    auto window = app.create_window(aardvark::Size{500, 500});
    auto document = app.get_document(window);

    auto text =
        UnicodeString((UChar*)u"Lorem ipsum dolor sit amet, consectetur");
    SkPaint paint;
    paint.setColor(SK_ColorWHITE);

    auto elem = std::make_shared<aardvark::elements::Align>(
        std::make_shared<aardvark::elements::FixedSize>(
            std::make_shared<aardvark::elements::Paragraph>(
                std::vector<std::shared_ptr<aardvark::inline_layout::Span>>{
                    std::make_shared<aardvark::inline_layout::TextSpan>(
                        text, paint)}),
            aardvark::Size{100 /* width */, 200 /* height */}),
        aardvark::value::abs(50), aardvark::value::abs(50));
    document->set_root(elem);
    // auto state = AppState{elem, background};
    // app.user_pointer = (void*)(&state);
    // app.event_handler = &handle_events;
    app.run();
};
