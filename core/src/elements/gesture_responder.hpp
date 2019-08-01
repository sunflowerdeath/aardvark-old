#pragma once

#include <memory>
#include <functional>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"
#include "../pointer_events/responder.hpp"

namespace aardvark::elements {

class GestureResponder : public SingleChildElement {
  public:
    class InnerResponder : public Responder {
      public:
        InnerResponder(GestureResponder* elem) : elem(elem){};
        GestureResponder* elem;
        void pointer_add(PointerEvent event) override {
            elem->pointer_add(event);
        };
        void pointer_update(PointerEvent event) override {
            elem->pointer_update(event);
        };
        void pointer_remove(PointerEvent event) override {
            elem->pointer_remove(event);
        };
    };

    GestureResponder(std::shared_ptr<Element> child, HitTestMode mode,
                     std::function<void(PointerEvent)> pointer_add,
                     std::function<void(PointerEvent)> pointer_update,
                     std::function<void(PointerEvent)> pointer_remove,
                     bool is_repaint_boundary = false)
        : SingleChildElement(child,
                             /* is_repaint_boundary */ is_repaint_boundary,
                             /* size_by_parent */ true),
          mode(mode),
          pointer_add(pointer_add),
          pointer_update(pointer_update),
          pointer_remove(pointer_remove),
          responder(InnerResponder(this)){};

    HitTestMode mode;
    std::function<void(PointerEvent)> pointer_add;
    std::function<void(PointerEvent)> pointer_update;
    std::function<void(PointerEvent)> pointer_remove;
    InnerResponder responder;

    std::string get_debug_name() override { return "GestureResponder"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
    HitTestMode get_hit_test_mode() override { return mode; };
    Responder* get_responder() override { return &responder; };
};

}  // namespace aardvark::elements
