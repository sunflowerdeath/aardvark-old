#include <memory>
#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include "SkPaint.h"
// #include <JavaScriptCore/JavaScript.h>
#include "elements/elements.hpp"
#include "inline_layout/text_span.hpp"
#include "platforms/android/android_app.hpp"

std::shared_ptr<aardvark::Element> create_elem() {
  auto red_box = std::make_shared<aardvark::elements::Align>(
      std::make_shared<aardvark::elements::FixedSize>(
          std::make_shared<aardvark::elements::Background>(SK_ColorRED),
          aardvark::Size{200, 200}),
      aardvark::elements::EdgeInsets{
          aardvark::Value::abs(200), // left
          aardvark::Value::abs(200)  // top
      });

    auto text =
        UnicodeString((UChar*)u"Lorem ipsum dolor sit amet, consectetur");
    SkPaint paint;
    paint.setColor(SK_ColorWHITE);
    paint.setTextSize(24);
    paint.setAntiAlias(true);
  auto span = std::make_shared<aardvark::inline_layout::TextSpan>(text, paint);
  auto paragraph = std::make_shared<aardvark::elements::Paragraph>(
      std::vector<std::shared_ptr<aardvark::inline_layout::Span>>{span},
      aardvark::inline_layout::LineMetrics::from_paint(paint).scale(1.5));
  auto aligned_paragraph = std::make_shared<aardvark::elements::Align>(
      std::make_shared<aardvark::elements::FixedSize>(paragraph,
                                                      aardvark::Size{200, 200}),
      aardvark::elements::EdgeInsets{
          aardvark::Value::abs(200), // left
          aardvark::Value::abs(500)  // top
      });
  auto stack = std::make_shared<aardvark::elements::Stack>(
      std::vector<std::shared_ptr<aardvark::Element>>{red_box,
                                                      aligned_paragraph});
  return stack;
}

// Main entry point of a native application that is using
// android_native_app_glue. It runs in its own thread, with its own event loop
// for receiving input events and doing other things.
void android_main(android_app* native_app) {
    eglBindAPI(EGL_OPENGL_ES_API);
    auto app = std::make_shared<aardvark::AndroidApp>(native_app);
    app->root = create_elem();;
    app->run();
}
