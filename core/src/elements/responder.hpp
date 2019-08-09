#pragma once

#include <memory>
#include <functional>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"
#include "../pointer_events/responder.hpp"

namespace aardvark::elements {

class ResponderElement : public SingleChildElement {
  public:
    class InnerResponder : public Responder {
      public:
        InnerResponder(ResponderElement* elem) : elem(elem){};
        ResponderElement* elem;
        void handler(PointerEvent event,
                     ResponderEventType event_type) override {
            if (elem->handler) elem->handler(event, event_type);
        };
    };

    ResponderElement(
        std::shared_ptr<Element> child, HitTestMode mode,
        std::function<void(PointerEvent, ResponderEventType)> handler,
        bool is_repaint_boundary = false)
        : SingleChildElement(child,
                             /* is_repaint_boundary */ is_repaint_boundary,
                             /* size_by_parent */ true),
          mode(mode),
          handler(handler),
          responder(InnerResponder(this)){};

    HitTestMode mode;
    std::function<void(PointerEvent, ResponderEventType)> handler;
    InnerResponder responder;

    std::string get_debug_name() override { return "Responder"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
    HitTestMode get_hit_test_mode() override { return mode; };
    Responder* get_responder() override { return &responder; };
};

}  // namespace aardvark::elements
