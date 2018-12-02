#include <vector>
#include <iostream>
#include <SDL.h>
#include "SkCanvas.h"
#include "base_types.hpp"
#include "window.hpp"
#include "compositing.hpp"
#include "document.hpp"
#include "elements/center.hpp"
#include "elements/fixed_size.hpp"
#include "elements/background.hpp"

void paint_frame(SkCanvas* canvas, int pos) {
  SkPaint paint;
  const char* message = "Hello world";
  paint.setColor(SK_ColorWHITE);
  canvas->drawCircle(200, 50 + pos, 25, paint);
  // canvas->drawText(message, strlen(message), SkIntToScalar(100),
                   // SkIntToScalar(100 + pos), paint);
}

int main() {
  auto window = aardvark::SdlWindow(aardvark::Size{500, 500});
  auto compositor = aardvark::Compositor(aardvark::Size{500, 500});
  // auto screen = compositor.get_screen_layer();

  auto root =
      std::make_shared<aardvark::elements::Center>(
          std::make_shared<aardvark::elements::FixedSize>(
              std::make_shared<aardvark::elements::Background>(SK_ColorGREEN),
              aardvark::Size{100, 100}));
  auto document = aardvark::Document(compositor, root);

  /*
  auto offscreen = compositor.create_offscreen_layer(aardvark::Size{200, 200});
  SkPaint paint;
  paint.setColor(SK_ColorGREEN);
  offscreen->canvas->drawCircle(100, 100, 100, paint);
  offscreen->compose_options.opacity = 0.5;
  */

  int pos = 0;
  bool quit = false;
  while (!quit) {
    int time = SDL_GetTicks();

    // handle events and update state
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          quit = true;
          break;
        default:
          break;
      }
    }

    if (pos > 300) pos = 0;
    pos++;

    // paint
    window.make_current();
    document.paint();
    /*
    screen->clear();
    paint_frame(screen->canvas, pos);
    compositor.paint_layer(screen.get(), offscreen.get(),
                           aardvark::Position{150, 200});
    screen->canvas->flush();
    */

    // For some reason swapping with vsync does not make frames 16ms,
    // frames are anywhere between 3 and 15ms, and additionally it 
    // constantly spends 5% cpu, so I disabled vsync and try to maintain 
    // 60 fps manually.
    window.swap_now();
    time = SDL_GetTicks() - time;
    if (time < 16) {
      // framerate is too high, need to wait
      SDL_Delay(16 - time);
    }
  }
  /*
  auto root = Aardvark::Elements::Stack(std::vector<Aardvark::Element>{
      Aardvark::Elements::Center(Aardvark::Elements::Background()),
      Aardvark::Elements::Align(
          Aardvark::Elements::FixedSize(
              Aardvark::Elements::Background(),
              Aardvark::Size(50, 75)
              ),
          Aardvark::Value(100, Aardvark::Unit::Px),  // left
          Aardvark::Value(200, Aardvark::Unit::Px)   // top
          ),
      Aardvark::Elements::Align(
          Aardvark::Elements::FixedSize(
              Aardvark::Elements::Background(),
              Aardvark::Size(50, 75)
              ),
          Aardvark::Value(75, Aardvark::Unit::Percent),  // left
          Aardvark::Value(75, Aardvark::Unit::Percent)   // top
          ),
  });
  auto document = Document(&compositor, root);
  document.paint();
  // auto jsContext = Aardvark::JSContext();
  // jsContext.run("???");
  */
  return 0;
}

