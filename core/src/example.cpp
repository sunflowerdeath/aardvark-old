#include <SDL.h>
#include <iostream>
#include <vector>
#include "SkCanvas.h"
#include "base_types.hpp"

#include "compositing.hpp"
#include "document.hpp"
#include "elements/elements.hpp"
#include "window.hpp"

int main() {
  auto size = aardvark::Size{500, 500};
  auto window = aardvark::SdlWindow(size);
  auto compositor = aardvark::Compositor(size);

  auto background =
      std::make_shared<aardvark::elements::Background>(SK_ColorBLUE);
  auto root = std::make_shared<aardvark::elements::Stack>(
      std::vector<std::shared_ptr<aardvark::Element>>{
          std::make_shared<aardvark::elements::Align>(
              std::make_shared<aardvark::elements::FixedSize>(
                  std::make_shared<aardvark::elements::Background>(
                      SK_ColorGREEN),
                  aardvark::Size{100, 100}),
              0, 200),
          std::make_shared<aardvark::elements::Center>(
              std::make_shared<aardvark::elements::FixedSize>(
                  background, aardvark::Size{100, 100}))});
  auto document = aardvark::Document(compositor, root);

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
        case SDL_MOUSEBUTTONDOWN:
          std::cout << "down" << std::endl;
          background->set_props(SK_ColorRED);
          break;
        case SDL_MOUSEBUTTONUP:
          std::cout << "up" << std::endl;
          background->set_props(SK_ColorBLUE);
          break;
        default:
          break;
      }
    }

    // paint
    window.make_current();
    document.paint();

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
  return 0;
}

