#pragma once

#include "../elements/responder.hpp"
#include "span.hpp"

namespace aardvark::inline_layout {

class ResponderSpan : public SingleChildSpan {
  public:
    ResponderSpan(
        std::shared_ptr<Span> child = nullptr,
        HitTestMode hit_test_mode = HitTestMode::PassToParent,
        std::function<void(PointerEvent, ResponderEventType)> handler = nullptr,
        std::optional<SpanBase> base_span = std::nullopt)
        : SingleChildSpan(std::move(child), base_span),
          hit_test_mode(hit_test_mode),
          handler(std::move(handler)){};

    InlineLayoutResult layout(InlineConstraints constraints) override {
        auto child_result = child->layout(constraints);
        if (child_result.type == InlineLayoutResult::Type::fit) {
            return InlineLayoutResult::fit(
                child_result.width, child_result.metrics, shared_from_this());
        } else if (child_result.type == InlineLayoutResult::Type::wrap) {
            return InlineLayoutResult::wrap(shared_from_this());
        } else {
            auto fit = std::make_shared<ResponderSpan>(
                child_result.fit_span.value(),
                hit_test_mode,
                handler,
                SpanBase{this, 0});
            auto remainder = std::make_shared<ResponderSpan>(
                child_result.remainder_span.value(),
                hit_test_mode,
                handler,
                SpanBase{
                    this, child_result.fit_span.value()->get_text_length()});
            return InlineLayoutResult::split(
                child_result.width, child_result.metrics, fit, remainder);
        }
    }

    std::shared_ptr<Element> render() override {
        return std::make_shared<ResponderElement>(
            child->render(), hit_test_mode, handler);
    }

    std::shared_ptr<Span> slice(int start, int end) override {
        return shared_from_this();
    }

    HitTestMode hit_test_mode;
    std::function<void(PointerEvent, ResponderEventType)> handler;
};

}  // namespace aardvark::inline_layout
