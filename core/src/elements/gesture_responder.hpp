#pragma once

#include <memory>
#include <functional>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"
#include "../responder.hpp"

namespace aardvark::elements {

class GestureResponder : public SingleChildElement {
  public:
    class InnerResponder : public Responder {
      public:
        InnerResponder(GestureResponder* elem) : elem(elem){};
        GestureResponder* elem;
        void start(PointerEvent event) override { elem->start(event); };
        void update(PointerEvent event) override { elem->update(event); };
        void end(PointerEvent event, bool is_terminated) override {
            elem->end(event, is_terminated);
        };
    };

    GestureResponder(std::shared_ptr<Element> child, ResponderMode mode,
                     std::function<void(PointerEvent)> start,
                     std::function<void(PointerEvent)> update,
                     std::function<void(PointerEvent, bool)> end,
                     bool is_repaint_boundary = false)
        : SingleChildElement(child,
                             /* is_repaint_boundary */ is_repaint_boundary,
                             /* size_by_parent */ true),
          mode(mode),
          start(start),
          update(update),
          end(end),
          responder(InnerResponder(this)){};

    ResponderMode mode;
    std::function<void(PointerEvent)> start;
    std::function<void(PointerEvent)> update;
    std::function<void(PointerEvent, bool)> end;
    InnerResponder responder;

    std::string get_debug_name() override { return "GestureResponder"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
    ResponderMode get_responder_mode() override { return mode; };
    Responder* get_responder() override { return &responder; };
};

}  // namespace aardvark::elements
