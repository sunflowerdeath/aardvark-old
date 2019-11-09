#include "Catch2/catch.hpp"
#include "../document.hpp"
#include "../elements/elements.hpp"

using namespace aardvark;

TEST_CASE("ElementObserver", "[element_observer]") {
    SECTION("SizeObserver") {
        auto gr_context = GrContext::MakeGL();
        auto screen = Layer::make_offscreen_layer(gr_context, Size{500, 500});
        auto document =
            std::make_shared<aardvark::Document>(gr_context, screen);

        auto small = aardvark::elements::SizeConstraints::exact(Value.abs(100),
                                                                Value.abs(100));
        auto big = aardvark::elements::SizeConstraints::exact(Value.abs(200),
                                                              Value.abs(200));
        auto child1 = std::make_shared<aardvark::elements::Sized>(
            std::make_shared<aardvark::elements::Background>(SkColor_RED),
            small);
        auto child2 = std::make_shared<aardvark::elements::Sized>(
            std::make_shared<aardvark::elements::Background>(SkColor_RED),
            small);
        auto root =
            std::make_shared<aardvark::elements::Stack>({child1, child2});
        document->set_root(root);
        document->render();

        // Handler called when element changed size
        auto connection = document->observe_element_size(child1);
        child1->size_constraints = big;
        child1->change();
        document->render();
        REQUIRE(); // called
        REQUIRE(); // correct size

        // Handler is not called if element did not relayouted
        child2->size_constraints = big;
        child2->change();
        document->render();
        REQUIRE(); // not called
        
        // Handler is not called if element did not changed size
        child1->size_constraints = big;
        child1->change();
        document->render();
        REQUIRE(); // not called

        // Handler is not called after disconnecting
        connection.disconnect();
        child1->size_constraints = small;
        child1->change();
        document->render();
        REQUIRE(); // not called
    }

    // bounding box observer
    // same as element +
    // called when parent layer transform updated
}
