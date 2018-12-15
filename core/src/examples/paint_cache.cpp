#include <chrono>
#include <thread>
#include <iostream>
#include <vector>

#include <GLFW/glfw3.h>

#include "../base_types.hpp"
#include "../desktop_window.hpp"
#include "../layer.hpp"
#include "../document.hpp"
#include "../elements/elements.hpp"

std::shared_ptr<aardvark::Element> make_elems() {
  auto border = std::make_shared<aardvark::elements::Border>(
      std::make_shared<aardvark::elements::Background>(SK_ColorWHITE),
      aardvark::elements::BoxBorders{
          aardvark::elements::BorderSide{4, SK_ColorRED},  // top
          aardvark::elements::BorderSide{4, SK_ColorRED},  // right
          aardvark::elements::BorderSide{4, SK_ColorRED},  // bottom
          aardvark::elements::BorderSide{4, SK_ColorRED},  // left
      },
      aardvark::elements::BoxRadiuses{
          aardvark::elements::Radius{4, 4},    // top_left
          aardvark::elements::Radius{8, 8},    // top_right
          aardvark::elements::Radius{16, 16},  // bottom_right
          aardvark::elements::Radius{32, 32}   // bottom_left
      });
  return std::make_shared<aardvark::elements::FixedSize>(
      border, aardvark::Size{50, 50});
};

int main() {
  auto window = aardvark::DesktopWindow(aardvark::Size{550, 550});

  auto background =
      std::make_shared<aardvark::elements::Background>(SK_ColorWHITE);
  std::vector<std::shared_ptr<aardvark::Element>> elements;
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      elements.push_back(std::make_shared<aardvark::elements::Align>(
          make_elems(),
          aardvark::value::abs(i * 55),  // left
          aardvark::value::abs(j * 55)   // top
          ));
    }
  }

  auto root = std::make_shared<aardvark::elements::Stack>(
      std::vector<std::shared_ptr<aardvark::Element>>{
          background,
          std::make_shared<aardvark::elements::Stack>(elements)});
  auto document = aardvark::Document(root);

  bool quit = false;
  while (!quit) {
    auto start = std::chrono::high_resolution_clock::now();

    glfwPollEvents();
    int state = glfwGetKey(window.window, GLFW_KEY_R);
    if (state == GLFW_PRESS) {
      std::cout << "R" << std::endl;
      background->set_props(SK_ColorLTGRAY);
    }
    state = glfwGetKey(window.window, GLFW_KEY_B);
    if (state == GLFW_PRESS) {
      std::cout << "B" << std::endl;
      background->set_props(SK_ColorWHITE);
    }

    // paint
    window.make_current();
    bool painted = document.paint();
    window.swap_now();

    auto end = std::chrono::high_resolution_clock::now();
    auto time =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start)
            .count();
    if (painted) {
      std::cout << "frame time: " << (time / 1000.0) << "ms" << std::endl;
    }
    if (time < 16000) {
      // framerate is too high, need to wait
      std::this_thread::sleep_for(std::chrono::microseconds(16000 - time));
    }
  }
  return 0;
}
