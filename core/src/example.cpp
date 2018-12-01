#include <vector>
#include <SDL.h>
#include "SkCanvas.h"
#include "window.hpp"
#include "compositing.hpp"

void paint1(SkCanvas* canvas, int pos) {
  SkPaint paint;
  const char* message = "Hello world";
  paint.setColor(SK_ColorWHITE);
  canvas->drawText(message, strlen(message), SkIntToScalar(100),
                   SkIntToScalar(100 + pos), paint);
  canvas->flush();
}

void paint2(SkCanvas* canvas, int pos) {
  SkPaint paint;
  const char* message = "Hello world";
  paint.setColor(SK_ColorGREEN);
  canvas->drawText(message, strlen(message), SkIntToScalar(100),
                   SkIntToScalar(100 + pos), paint);
  canvas->flush();
}

int main() {
  auto window1 = aardvark::SdlWindow(aardvark::Size{500, 500});
  auto window2 = aardvark::SdlWindow(aardvark::Size{800, 600});

  auto compositor1 = aardvark::compositing::Compositor(aardvark::Size{500, 500});
  auto compositor2 = aardvark::compositing::Compositor(aardvark::Size{800, 600});
  auto screen1 = compositor1.getScreenLayer();
  auto screen2 = compositor1.getScreenLayer();

  int pos = 0;
  bool quit = false;
  while (!quit) {
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

    window1.make_current();
    screen1->clear();
    paint1(screen1->canvas, pos);
    window1.swap_now();

    window2.make_current();
    paint2(screen2->canvas, pos);
    window2.swap();

    if (pos > 100) pos = 0;
    pos++;


  }
  // window1.handleEvents();
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

