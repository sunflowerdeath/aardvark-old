#include "spdlog/spdlog.h"
#include "../base_types.hpp"
#include "../elements/elements.hpp"
#include "../platforms/desktop/desktop_app.hpp"

std::shared_ptr<aardvark::Element> make_border(
    std::shared_ptr<aardvark::Element> elem) {
    return std::make_shared<aardvark::elements::Border>(
        elem,
        aardvark::elements::BoxBorders::all(
            aardvark::elements::BorderSide{1, SK_ColorBLACK}),
        aardvark::elements::BoxRadiuses::all(
            aardvark::elements::Radius::circular(0)));
}

std::shared_ptr<aardvark::Element> make_align(
    std::shared_ptr<aardvark::Element> elem, float left, float top) {
    return std::make_shared<aardvark::elements::Align>(
        elem, aardvark::elements::EdgeInsets{aardvark::Value::abs(left),
                                             aardvark::Value::abs(top)});
}

std::shared_ptr<aardvark::Element> make_bg(SkColor color) {
    return std::make_shared<aardvark::elements::Background>(color);
}

std::shared_ptr<aardvark::Element> make_size(
    std::shared_ptr<aardvark::Element> elem, float width, float height) {
    return std::make_shared<aardvark::elements::Sized>(
        elem, aardvark::elements::SizeConstraints{
                  aardvark::Value::abs(width), aardvark::Value::abs(height)});
}

std::shared_ptr<aardvark::Element> make_container(
    std::shared_ptr<aardvark::Element> elem, float left, float top, float width,
    float height) {
    return make_align(make_border(make_size(elem, width, height)), left, top);
}

std::vector<std::shared_ptr<aardvark::Element>> make_children() {
    return std::vector<std::shared_ptr<aardvark::Element>>{
        make_size(make_bg(SK_ColorRED), 50, 50),
        make_size(make_bg(SK_ColorGREEN), 50, 50),
        make_size(make_bg(SK_ColorBLUE), 50, 50),
    };
}

int main() {
    auto event_loop = std::make_shared<aardvark::EventLoop>();
    auto app = aardvark::DesktopApp(event_loop);
    auto window = app.create_window(aardvark::Size{500, 500});
    auto document = app.get_document(window);

    auto default_elem = std::make_shared<aardvark::elements::Flex>(
        make_children(),
        aardvark::elements::FlexDirection::row,
        aardvark::elements::FlexJustify::start,
        aardvark::elements::FlexAlign::start);

    auto justify_end = std::make_shared<aardvark::elements::Flex>(
        make_children(),
        aardvark::elements::FlexDirection::row,
        aardvark::elements::FlexJustify::end,
        aardvark::elements::FlexAlign::start);

    auto justify_evenly = std::make_shared<aardvark::elements::Flex>(
        make_children(),
        aardvark::elements::FlexDirection::row,
        aardvark::elements::FlexJustify::space_evenly,
        aardvark::elements::FlexAlign::start);

    auto stack = std::make_shared<aardvark::elements::Stack>(
        std::vector<std::shared_ptr<aardvark::Element>>{
            make_bg(SK_ColorWHITE),
            make_container(default_elem, 20, 20, 300, 100),
            make_container(justify_end, 20, 140, 300, 100),
            make_container(justify_evenly, 20, 260, 300, 100)
        });

    document->set_root(stack);

    spdlog::set_level(spdlog::level::debug);
    app.run();
    event_loop->run();
};
