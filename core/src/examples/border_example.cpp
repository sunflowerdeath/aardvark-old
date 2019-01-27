#include <iostream>
#include <unicode/unistr.h>
#include "../base_types.hpp"
#include "../platforms/desktop/desktop_app.hpp"
#include "../elements/elements.hpp"

int main() {
    auto app = aardvark::DesktopApp();
    auto window = app.create_window(aardvark::Size{500, 500});
    auto document = app.get_document(window);

    auto single = std::make_shared<aardvark::elements::Border>(
        std::make_shared<aardvark::elements::Background>(SK_ColorLTGRAY),
        aardvark::elements::BoxBorders::all(
            aardvark::elements::BorderSide{/* width */ 1,
                                           /* color */ SK_ColorBLACK}),
        aardvark::elements::BoxRadiuses::all(
            aardvark::elements::Radius::circular(0)));

    auto different = std::make_shared<aardvark::elements::Border>(
        std::make_shared<aardvark::elements::Background>(SK_ColorLTGRAY),
        aardvark::elements::BoxBorders{
            aardvark::elements::BorderSide{/* width */ 1,
                                           /* color */ SK_ColorRED},  // top
            aardvark::elements::BorderSide{/* width */ 1,
                                           /* color */ SK_ColorGREEN},  // right
            aardvark::elements::BorderSide{/* width */ 1,
                                           /* color */ SK_ColorBLUE},  // bottom
            aardvark::elements::BorderSide{/* width */ 1,
                                           /* color */ SK_ColorGRAY}  // left
        },
        aardvark::elements::BoxRadiuses::all(
            aardvark::elements::Radius::circular(0)));

    auto different2 = std::make_shared<aardvark::elements::Border>(
        std::make_shared<aardvark::elements::Background>(SK_ColorLTGRAY),
        aardvark::elements::BoxBorders{
            aardvark::elements::BorderSide{/* width */ 1,
                                           /* color */ SK_ColorRED},  // top
            aardvark::elements::BorderSide{/* width */ 5,
                                           /* color */ SK_ColorGREEN},  // right
            aardvark::elements::BorderSide{/* width */ 10,
                                           /* color */ SK_ColorBLUE},  // bottom
            aardvark::elements::BorderSide{/* width */ 0,
                                           /* color */ SK_ColorWHITE}  // left
        },
        aardvark::elements::BoxRadiuses::all(
            aardvark::elements::Radius::circular(0)));

    auto rounded = std::make_shared<aardvark::elements::Border>(
        std::make_shared<aardvark::elements::Background>(SK_ColorLTGRAY),
        aardvark::elements::BoxBorders::all(
            aardvark::elements::BorderSide{/* width */ 2,
                                           /* color */ SK_ColorRED}),
        aardvark::elements::BoxRadiuses::all(
            aardvark::elements::Radius::circular(8)));

    auto rounded2 = std::make_shared<aardvark::elements::Border>(
        std::make_shared<aardvark::elements::Background>(SK_ColorLTGRAY),
        aardvark::elements::BoxBorders::all(
            aardvark::elements::BorderSide{/* width */ 10,
                                           /* color */ SK_ColorRED}),
        aardvark::elements::BoxRadiuses{
            aardvark::elements::Radius::circular(0),  // top_left
            aardvark::elements::Radius::circular(5),  // top_right
            aardvark::elements::Radius::circular(10),   // bottom_right
            aardvark::elements::Radius::circular(20)    // bottom_left
        });

    auto root = std::make_shared<aardvark::elements::Stack>(
        std::vector<std::shared_ptr<aardvark::Element>>{
            std::make_shared<aardvark::elements::Background>(SK_ColorWHITE),

            std::make_shared<aardvark::elements::Align>(
                std::make_shared<aardvark::elements::FixedSize>(
                    single, aardvark::Size{100, 150}),
                aardvark::elements::EdgeInsets{
                aardvark::Value::abs(50),  // left
                aardvark::Value::abs(50)   // top
                }),

            std::make_shared<aardvark::elements::Align>(
                std::make_shared<aardvark::elements::FixedSize>(
                    different, aardvark::Size{100, 150}),
                aardvark::elements::EdgeInsets{
                aardvark::Value::abs(200),  // left
                aardvark::Value::abs(50)    // top
                }),

            std::make_shared<aardvark::elements::Align>(
                std::make_shared<aardvark::elements::FixedSize>(
                    different2, aardvark::Size{100, 150}),
                aardvark::elements::EdgeInsets{
                aardvark::Value::abs(350),  // left
                aardvark::Value::abs(50)    // top
                }),

            std::make_shared<aardvark::elements::Align>(
                std::make_shared<aardvark::elements::FixedSize>(
                    rounded, aardvark::Size{100, 150}),
                aardvark::elements::EdgeInsets{
                    aardvark::Value::abs(50),  // left
                    aardvark::Value::abs(250)  // top
                }),

            std::make_shared<aardvark::elements::Align>(
                std::make_shared<aardvark::elements::FixedSize>(
                    rounded2, aardvark::Size{100, 150}),
                aardvark::elements::EdgeInsets{
                aardvark::Value::abs(200),  // left
                aardvark::Value::abs(250)   // top
                }),

        });

    document->set_root(root);
    app.run();
};
