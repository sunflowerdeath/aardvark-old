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

std::shared_ptr<aardvark::Element> make_elems(bool circle) {
    auto clipper = [](aardvark::Size size) {
        SkPath clip_path;
        clip_path.addCircle(50, 50, 40);
        return clip_path;
    };
    auto red = std::make_shared<aardvark::elements::Background>(SK_ColorRED);
    auto blue = std::make_shared<aardvark::elements::Background>(SK_ColorBLUE);
    return std::make_shared<aardvark::elements::FixedSize>(
        std::make_shared<aardvark::elements::Clip>(
            std::make_shared<aardvark::elements::Stack>(
                std::vector<std::shared_ptr<aardvark::Element>>{
                    red, std::make_shared<aardvark::elements::Align>(
                             std::make_shared<aardvark::elements::FixedSize>(
                                 blue, aardvark::Size{100, 100}),
                             aardvark::value::abs(50),  // left
                             aardvark::value::abs(50)   // top
                             )},
                true),
            circle ? clipper : aardvark::elements::Clip::default_clip),
        aardvark::Size{100, 100});
};

int main() {
    auto window = aardvark::DesktopWindow(aardvark::Size{500, 500});
    auto background =
        std::make_shared<aardvark::elements::Background>(SK_ColorWHITE);
    auto elements = std::vector<std::shared_ptr<aardvark::Element>>{background};

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            elements.push_back(std::make_shared<aardvark::elements::Align>(
                make_elems(i % 2 == 0),
                aardvark::value::abs(i * 110),  // left
                aardvark::value::abs(j * 110)   // top
                ));
        }
    }

    auto clipper = [](aardvark::Size size) {
        SkPath clip_path;
        clip_path.addOval({0, 0, 500, 500});
        return clip_path;
    };
    auto root = std::make_shared<aardvark::elements::Clip>(
        std::make_shared<aardvark::elements::Stack>(elements, true), clipper);
    auto document = aardvark::Document(root);

    bool quit = false;
    // glfwSetKeyCallback(window, key_callback);
    while (!quit) {
        auto start = std::chrono::high_resolution_clock::now();

        // handle events and update state
        glfwPollEvents();

        int state = glfwGetKey(window.window, GLFW_KEY_B);
        if (state == GLFW_PRESS) {
            std::cout << "B" << std::endl;
            background->set_props(SK_ColorLTGRAY);
            // background2->set_props(SK_ColorBLUE);
        }
        state = glfwGetKey(window.window, GLFW_KEY_R);
        if (state == GLFW_PRESS) {
            std::cout << "R" << std::endl;
            background->set_props(SK_ColorWHITE);
        }

        // paint
        window.make_current();
        bool painted = document.paint();
        window.swap_now();

        // For some reason swapping with vsync does not make frames 16ms,
        // frames are anywhere between 3 and 15ms, and additionally it
        // constantly spends 5% cpu, so I disabled vsync and try to maintain
        // about 60 fps manually.
        auto end = std::chrono::high_resolution_clock::now();
        auto time =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start)
                .count();
        if (painted) {
            std::cout << "frame time: " << (time / 1000.0) << "ms" << std::endl;
        }
        if (time < 16000) {
            // framerate is too high, need to wait
            std::this_thread::sleep_for(
                std::chrono::microseconds(16000 - time));
        }
    }
    return 0;
}

