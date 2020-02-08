#include <Catch2/catch.hpp>

#include <GLFW/glfw3.h>
#include "../document.hpp"
#include "../elements/elements.hpp"
#include "../platforms/desktop/desktop_window.hpp"

using namespace aardvark;

TEST_CASE("ElementObserver", "[element_observer]") {
    // Creating window is needed to have opengl context
    glfwInit();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    auto window =
        std::make_shared<aardvark::DesktopWindow>(nullptr, Size{500, 500});

    SECTION("SizeObserver") {
        auto gr_context = GrContext::MakeGL();
        auto screen = Layer::make_offscreen_layer(gr_context, Size{500, 500});
        auto document =
            std::make_shared<aardvark::Document>(gr_context, screen);

        auto small = aardvark::elements::SizeConstraints::exact(
            Value::abs(100), Value::abs(100));
        auto big = aardvark::elements::SizeConstraints::exact(Value::abs(200),
                                                              Value::abs(200));
        auto child1 = std::make_shared<aardvark::elements::Sized>(
            std::make_shared<aardvark::elements::Background>(SK_ColorRED),
            small);
        auto child2 = std::make_shared<aardvark::elements::Sized>(
            std::make_shared<aardvark::elements::Background>(SK_ColorRED),
            small);
        auto root = std::make_shared<aardvark::elements::Stack>(
            std::vector<std::shared_ptr<aardvark::Element>>{child1, child2});
        document->set_root(root);
        document->render();

        // Handler called when element changed size
        auto handler_called = false;
        auto handler_arg = Size{0, 0};
        auto reset = [&]() {
            handler_called = false;
            handler_arg = Size{0, 0};
        };
        auto handler = [&](Size size) {
            handler_called = true;
            handler_arg = size;
        };
        auto connection = document->observe_element_size(child1, handler);
        child1->size_constraints = big;
        child1->change();
        document->render();
        REQUIRE(handler_called);
        REQUIRE(handler_arg == Size{200, 200});
        reset();

        // Handler is not called if element did not changed size
        child1->size_constraints = big;
        child1->change();
        child2->size_constraints = big;
        child2->change();
        document->render();
        REQUIRE(!handler_called);
        reset();

        // Handler is not called after disconnecting
        connection.disconnect();
        child1->size_constraints = small;
        child1->change();
        document->render();
        REQUIRE(!handler_called);
        REQUIRE(child1->size == Size{100, 100});
        reset();
    }

    // TODO bounding box observer
}
