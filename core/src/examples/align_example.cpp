#include <iostream>
#include <stdio.h>
#include "SkPaint.h"
#include "../base_types.hpp"
#include "../platforms/desktop/desktop_app.hpp"
#include "../elements/elements.hpp"

std::shared_ptr<aardvark::Element> make_box(
    std::shared_ptr<aardvark::Element> elem, float left, float top) {
    auto stack = std::make_shared<aardvark::elements::Stack>(
        std::vector<std::shared_ptr<aardvark::Element>>{
            std::make_shared<aardvark::elements::Background>(SK_ColorLTGRAY),
            elem});
    return std::make_shared<aardvark::elements::Align>(
        std::make_shared<aardvark::elements::FixedSize>(
            stack, aardvark::Size{100, 100}),
        aardvark::elements::EdgeInsets{aardvark::Value::abs(left),
                                       aardvark::Value::abs(top)});
};

int main() {
    auto app = aardvark::DesktopApp();
    auto window = app.create_window(aardvark::Size{500, 500});
    auto document = app.get_document(window);

    auto left_top = std::make_shared<aardvark::elements::Align>(
        std::make_shared<aardvark::elements::Background>(SK_ColorBLUE),
        aardvark::elements::EdgeInsets{
            aardvark::Value::abs(20),  // left
            aardvark::Value::abs(20)   // top
        });

    auto right_bottom = std::make_shared<aardvark::elements::Align>(
        std::make_shared<aardvark::elements::Background>(SK_ColorBLUE),
        aardvark::elements::EdgeInsets{
            aardvark::Value::none(),   // left
            aardvark::Value::none(),   // top
            aardvark::Value::abs(20),  // right
            aardvark::Value::abs(20)   // bottom
        });

    auto padding = std::make_shared<aardvark::elements::Align>(
        std::make_shared<aardvark::elements::Background>(SK_ColorBLUE),
        aardvark::elements::EdgeInsets::all(aardvark::Value::abs(20)));

    auto root = std::make_shared<aardvark::elements::Stack>(
        std::vector<std::shared_ptr<aardvark::Element>>{
            std::make_shared<aardvark::elements::Background>(SK_ColorWHITE),
            make_box(left_top, 50, 50), make_box(right_bottom, 50, 200),
            make_box(padding, 50, 350)});

    document->set_root(root);
    app.run();
};
