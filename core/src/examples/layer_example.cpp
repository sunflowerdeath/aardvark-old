#include "spdlog/spdlog.h"
#include "../base_types.hpp"
#include "../elements/elements.hpp"
#include "../platforms/desktop/desktop_app.hpp"

struct AppState {
    std::shared_ptr<aardvark::elements::Layer> layer;
    aardvark::elements::TransformOptions transform;
};

// TODO abs and relative origin
void to_matrix(aardvark::elements::TransformOptions transform,
               SkMatrix* matrix) {
    matrix->setRotate(transform.rotation, 50, 50);
    matrix->postTranslate(transform.translate.left, transform.translate.top);
    matrix->postScale(transform.scale.horiz, transform.scale.vert,
                      transform.translate.left + 50,
                      transform.translate.top + 50);
};

void handle_events(aardvark::DesktopApp* app, aardvark::Event event) {
    auto state = (AppState*)app->user_pointer;
    if (auto key = std::get_if<aardvark::KeyEvent>(&event)) {
        if (key->action == aardvark::KeyAction::Press ||
            key->action == aardvark::KeyAction::Repeat) {
            if (key->key == GLFW_KEY_LEFT) {
                state->transform.translate.left -= 10;
            } else if (key->key == GLFW_KEY_RIGHT) {
                state->transform.translate.left += 10;
            } else if (key->key == GLFW_KEY_UP) {
                state->transform.translate.top -= 10;
            } else if (key->key == GLFW_KEY_DOWN) {
                state->transform.translate.top += 10;
            } else if (key->key == GLFW_KEY_Q) {
                state->transform.rotation -= 2;
            } else if (key->key == GLFW_KEY_W) {
                state->transform.rotation += 2;
            } else if (key->key == GLFW_KEY_Z) {
                state->transform.scale.horiz += 0.1;
                state->transform.scale.vert += 0.1;
            } else if (key->key == GLFW_KEY_X) {
                state->transform.scale.horiz -= 0.1;
                state->transform.scale.vert -= 0.1;
            }
        }

        SkMatrix matrix;
        to_matrix(state->transform, &matrix);
        state->layer->transform = matrix;
        state->layer->document->change_element(state->layer.get());
    }
}

int main() {
    auto event_loop = std::make_shared<aardvark::EventLoop>();
    auto app = aardvark::DesktopApp(event_loop);
    auto window = app.create_window(aardvark::Size{500, 500});
    auto document = app.get_document(window);

    auto state = AppState{};

    auto transform = aardvark::elements::TransformOptions{
        aardvark::Position{0, 0},  // translate
        {1.0, 1.0},                // scale
        45                         // rotate
    };
    SkMatrix matrix;
    to_matrix(transform, &matrix);

    auto background =
        std::make_shared<aardvark::elements::Background>(SK_ColorWHITE);
    auto handler = [background](aardvark::PointerEvent event,
                                aardvark::ResponderEventType event_type) {
        if (event_type == aardvark::ResponderEventType::add) {
            background->color = SK_ColorBLUE;
            background->change();
        } else if (event_type == aardvark::ResponderEventType::remove) {
            background->color = SK_ColorWHITE;
            background->change();
        }
    };

    auto layer_background =
        std::make_shared<aardvark::elements::Background>(SK_ColorRED);
    auto layer_handler = [layer_background](
                             aardvark::PointerEvent event,
                             aardvark::ResponderEventType event_type) {
        if (event_type == aardvark::ResponderEventType::add) {
            layer_background->color = SK_ColorGREEN;
            layer_background->change();
        } else if (event_type == aardvark::ResponderEventType::remove) {
            layer_background->color = SK_ColorRED;
            layer_background->change();
        }
    };

    auto layer = std::make_shared<aardvark::elements::Layer>(
        std::make_shared<aardvark::elements::ResponderElement>(
            layer_background,                     // child
            aardvark::HitTestMode::PassToParent,  // mode
            layer_handler),                       // handler
        matrix);
    auto layer_align = std::make_shared<aardvark::elements::Center>(
        std::make_shared<aardvark::elements::FixedSize>(
            layer, aardvark::Size{200 /* width */, 200 /* height */}));

    auto responder = std::make_shared<aardvark::elements::ResponderElement>(
        background,                           // child
        aardvark::HitTestMode::PassToParent,  // mode
        handler);
    auto align = std::make_shared<aardvark::elements::Align>(
        std::make_shared<aardvark::elements::FixedSize>(
            responder, aardvark::Size{200 /* width */, 200 /* height */}),
        aardvark::elements::EdgeInsets{aardvark::Value::abs(20),
                                       aardvark::Value::abs(20)});

    auto stack = std::make_shared<aardvark::elements::Stack>(
        std::vector<std::shared_ptr<aardvark::Element>>{layer_align, align});

    document->set_root(stack);

    state.layer = layer;
    state.transform = transform;

    spdlog::set_level(spdlog::level::debug);
    app.user_pointer = (void*)(&state);
    app.event_handler = &handle_events;
    app.run();
    event_loop->run();
};
