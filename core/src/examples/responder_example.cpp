#include <stdio.h>
#include <iostream>
#include "../base_types.hpp"
#include "../platforms/desktop/desktop_app.hpp"
#include "../elements/elements.hpp"

struct AppState {
    std::shared_ptr<aardvark::elements::Align> align;
    std::shared_ptr<aardvark::elements::Background> background;
};

void rhandler(std::shared_ptr<aardvark::elements::Background> bg,
                     aardvark::PointerEvent event, int hz) {
    std::cout << "[button] start" << std::endl;

    if (hz == 0) {
        // if (event.action == aardvark::PointerEvent::Action::pointer_down) {
            bg->color = SK_ColorBLUE;
            bg->change();
        // }
    } else if (hz == 1) {
        if (event.action == aardvark::PointerAction::button_press) {
            std::cout << "[button] press" << std::endl;
        }

        if (event.action == aardvark::PointerAction::button_release) {
            std::cout << "[button] release" << std::endl;
        }
    } else {
        std::cout << "[button] end" << std::endl;
        bg->color = SK_ColorRED;
        bg->change();
    }
}

std::shared_ptr<aardvark::Element> create_button() {
    auto bg = std::make_shared<aardvark::elements::Background>(SK_ColorRED);
    auto handler = [bg](aardvark::PointerEvent event, int hz) {
        rhandler(bg, event, hz);
    };
    auto responder = std::make_shared<aardvark::elements::ResponderElement>(
        bg,                                   // child
        aardvark::HitTestMode::PassToParent,  // mode
        handler                               // handler
    );
    auto size = std::make_shared<aardvark::elements::FixedSize>(
        responder, aardvark::Size{200, 50});
    return size;
}

void before(aardvark::PointerEvent event) {
    if (event.action == aardvark::PointerAction::button_press) {
        std::cout << "before handler" << std::endl;
    }
}

void after(aardvark::PointerEvent event) {
    if (event.action == aardvark::PointerAction::button_press) {
        std::cout << "after handler" << std::endl;
    }
}

int main() {
    auto app = aardvark::DesktopApp();
    auto window = app.create_window(aardvark::Size{500, 500});
    auto document = app.get_document(window);

    auto button = create_button();

    auto stack = std::make_shared<aardvark::elements::Stack>(
        std::vector<std::shared_ptr<aardvark::Element>>{
            std::make_shared<aardvark::elements::Align>(
                button, aardvark::elements::EdgeInsets{
                            /* left */ aardvark::Value::abs(100),
                            /* top */ aardvark::Value::abs(100)})}

    );

    document->pointer_event_manager->add_handler(before, false);
    document->pointer_event_manager->add_handler(after, true);

    document->set_root(stack);
    // auto state = AppState{elem, background};
    // app.user_pointer = (void*)(&state);
    // app.event_handler = &handle_events;
    app.run();
};
