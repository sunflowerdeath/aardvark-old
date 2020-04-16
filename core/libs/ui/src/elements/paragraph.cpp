#include "elements/paragraph.hpp"

namespace aardvark {

ParagraphElement::ParagraphElement(
    // std::vector<std::shared_ptr<inline_layout::Span>> children,
    std::shared_ptr<inline_layout::Span> root,
    inline_layout::LineMetrics metrics,
    bool is_repaint_boundary)
    : root(std::move(root)),
      metrics(metrics),
      Element(is_repaint_boundary, /* size_depends_on_parent */ true){};

void ParagraphElement::next_line() {
    current_line = &lines.emplace_back();
    remaining_width = total_width;
};

Size ParagraphElement::layout(BoxConstraints constraints) {
    total_width = constraints.max_width;
    lines.clear();
    next_line();
    layout_span(root);

    elements.clear();
    current_height = 0;
    for (auto& line : lines) {
        auto line_metrics = inline_layout::calc_combined_metrics(line, metrics);
        inline_layout::render_spans(
            line, line_metrics, Position{0, current_height}, &elements);
        current_height += line_metrics.height;
    }

    for (auto& elem : elements) {
        elem->parent = this;
        document->layout_element(elem.get(), constraints);
    }

    return Size{constraints.max_width, current_height};
};

void ParagraphElement::layout_span(
    std::shared_ptr<inline_layout::Span> span_sp) {
    auto constraints = inline_layout::InlineConstraints{
        remaining_width,  // remaining_line_width
        total_width,      // total_line_width
        0,                // padding_before
        0                 // padding_after
    };
    auto result = span_sp->layout(constraints);
    if (result.fit_span != std::nullopt) {
        auto fit_span = result.fit_span.value();
        fit_span->metrics = result.metrics;
        fit_span->width = result.width;
        current_line->push_back(fit_span);
        remaining_width -= result.width;
    }
    if (result.remainder_span != std::nullopt) {
        next_line();
        layout_span(result.remainder_span.value());
    }
};

void ParagraphElement::paint(bool is_changed) {
    for (auto& elem : elements) document->paint_element(elem.get());
};

}  // namespace aardvark
