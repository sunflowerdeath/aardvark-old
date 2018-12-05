#include <chrono>
#include <thread>
#include <iostream>
#include <vector>

#include <GLFW/glfw3.h>
#include "SkCanvas.h"

#include "base_types.hpp"
#include "window.hpp"
#include "compositing.hpp"
#include "document.hpp"
#include "elements/elements.hpp"

int main() {
  auto size = aardvark::Size{500, 500};
  auto window = aardvark::GlfwWindow(size);
  auto compositor = aardvark::Compositor(size);
  auto background =
      std::make_shared<aardvark::elements::Background>(SK_ColorBLUE);
  auto root = std::make_shared<aardvark::elements::Stack>(
      std::vector<std::shared_ptr<aardvark::Element>>{
          std::make_shared<aardvark::elements::Align>(
              std::make_shared<aardvark::elements::Border>(
                  std::make_shared<aardvark::elements::FixedSize>(
                      std::make_shared<aardvark::elements::Background>(
                          SK_ColorWHITE),
                      aardvark::Size{100, 100}),
                  aardvark::elements::BoxBorders{
                      aardvark::elements::BorderSide{8, SK_ColorRED},  // top
                      aardvark::elements::BorderSide{8, SK_ColorRED},  // right
                      aardvark::elements::BorderSide{8,
                                                     SK_ColorWHITE},   // bottom
                      aardvark::elements::BorderSide{8, SK_ColorRED},  // left
                  }),
              aardvark::value::none(),    // left
              aardvark::value::none(),    // top
              aardvark::value::rel(0.1),  // right
              aardvark::value::rel(0.25)  // bottom
              ),
          std::make_shared<aardvark::elements::Center>(
              std::make_shared<aardvark::elements::FixedSize>(
                  background, aardvark::Size{100, 100}))});
  auto document = aardvark::Document(compositor, root);

  /*
  auto size2 = aardvark::Size{400, 400};
  auto window2 = aardvark::GlfwWindow(size2);
  auto compositor2 = aardvark::Compositor(size2);
  auto background2 =
      std::make_shared<aardvark::elements::Background>(SK_ColorBLUE);
  auto root2 = std::make_shared<aardvark::elements::Stack>(
      std::vector<std::shared_ptr<aardvark::Element>>{
          std::make_shared<aardvark::elements::Center>(
              std::make_shared<aardvark::elements::FixedSize>(
                  background2, aardvark::Size{100, 100}))});
  auto document2 = aardvark::Document(compositor2, root2);
  */

  SkPaint paint;

  bool quit = false;
  int pos = 0;

  // glfwSetKeyCallback(window, key_callback);
  while (!quit) {
    auto start = std::chrono::high_resolution_clock::now();

    // handle events and update state
    glfwPollEvents();

    pos++;
    if (pos > 200) pos = 0;

    int state = glfwGetKey(window.window, GLFW_KEY_B);
    if (state == GLFW_PRESS) {
      std::cout << "B" << std::endl;
      background->set_props(SK_ColorBLUE);
      // background2->set_props(SK_ColorBLUE);
    }
    state = glfwGetKey(window.window, GLFW_KEY_R);
    if (state == GLFW_PRESS) {
      std::cout << "R" << std::endl;
      background->set_props(SK_ColorRED);
      // background2->set_props(SK_ColorRED);
    }

    // paint
    window.make_current();
    document.paint();
    window.swap_now();

    /*
    window2.make_current();
    document2.paint();
    window2.swap_now();
    */

    // For some reason swapping with vsync does not make frames 16ms,
    // frames are anywhere between 3 and 15ms, and additionally it
    // constantly spends 5% cpu, so I disabled vsync and try to maintain
    // 60 fps manually.
    auto end = std::chrono::high_resolution_clock::now();
    auto time =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    if (time < 16) {
      // framerate is too high, need to wait
      std::this_thread::sleep_for(std::chrono::milliseconds(16 - time));
    }
  }
  return 0;
}

