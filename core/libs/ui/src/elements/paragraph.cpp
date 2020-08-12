#include "elements/paragraph.hpp"

namespace aardvark {

ParagraphElement::ParagraphElement(
    // std::vector<std::shared_ptr<inline_layout::Span>> children,
    std::shared_ptr<inline_layout::Span> root,
    inline_layout::LineMetrics metrics,
    bool is_repaint_boundary)
    : root(std::move(root)),
      metrics(metrics),
      Element(is_repaint_boundary, /* size_depends_on_parent */ false){};

void ParagraphElement::next_line() {
    current_line = &lines.emplace_back();
    remaining_width = total_width;
}

float ParagraphElement::layout_inline(float max_width) {
    total_width = max_width;
    lines.clear();
    next_line();
    layout_span(root);

    elements.clear();
    auto current_height = 0.0f;
    for (auto& line : lines) {
        auto line_metrics = inline_layout::calc_combined_metrics(line, metrics);
        inline_layout::render_spans(
            line, line_metrics, Position{0, current_height}, &elements, this);
        current_height += line_metrics.height;
    }

    return current_height;
}

Size ParagraphElement::layout(BoxConstraints constraints) {
    auto height = layout_inline(constraints.max_width);

    for (auto& elem : elements) {
        elem->set_document(document);
        document->layout_element(elem.get(), constraints);
    }

    return Size{constraints.max_width, height};
}

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
}

void ParagraphElement::paint(bool is_changed) {
    for (auto& elem : elements) document->paint_element(elem.get());
}

void ParagraphElement::visit_children(ChildrenVisitor visitor) {
    for (auto& elem : elements) visitor(elem);
}

float ParagraphElement::get_intrinsic_height(float width) {
    return layout_inline(width);
}

float ParagraphElement::get_intrinsic_width(float height) {
    // TODO does not really make sense
    return 0;
}

/*
std::vector<int> ParagraphElement::get_offset_at_position(Position pos) {
    auto res = std::vector<int>();
    auto current_height = 0.0f;
    ParagraphLine* current_line = nullptr;
    for (auto& line : lines) {
        current_height += line.metrics.height;
        current_line = &line;
        if (pos.top < current_height) break;
    }
    std::shared_ptr<inline_layout::Span> current_span = nullptr;
    auto current_width = 0.0f;
    for (auto& span : current_line->spans) {
        current_width += span.size.width;
        if (pos.left < current_width) break;
    }
    inline_layout::Span* base_span = nullptr;
    auto prev_offset = 0.0f;
    auto current_base = &current_span->base;
    while (current_base != nullptr) {
        prev_offset += current_base.prev_offset;
        current_base = &current_base->span->base;
    }
    auto offset = prev_offset + find_break_offset(glyph_widths, current_width);
    auto path = get_span_path();
    path.push_back(offset);
    return path;
}
*/

}  // namespace aardvark
