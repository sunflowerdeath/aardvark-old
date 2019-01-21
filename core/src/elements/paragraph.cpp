#include "paragraph.hpp"

namespace aardvark::elements {

float ParagraphLine::render(Position position) {
    elements.reserve(spans.size());

    float max_ascent = paragraph->metrics.baseline;
    float max_descent = paragraph->metrics.height - paragraph->metrics.baseline;
    for (auto& span : spans) {
        if (span->metrics.baseline > max_ascent) {
            max_ascent = span->metrics.baseline;
        }
        auto descent = span->metrics.height - span->metrics.baseline;
        if (descent > max_descent) max_descent = descent;
    }
    auto line_metrics = inline_layout::LineMetrics{
        max_ascent + max_descent,    // height
        max_ascent,                  // baseline
        paragraph->metrics.x_height  // x_height
    };

    auto current_width = 0.0f;
    for (auto& span : spans) {
        auto elem = span->render(/* selection */ std::nullopt);
        elem->parent = paragraph;
        auto constraints = BoxConstraints::from_size(
            Size{span->width, span->metrics.height}, /* tight */ true);
        auto size =
            paragraph->document->layout_element(elem.get(), constraints);
        elem->size = size;
        elem->rel_position =
            position + Position{current_width,
                                span->vert_align(line_metrics, span->metrics)};
        elements.push_back(elem);
        current_width += span->width;
    }

    return line_metrics.height;
};

Paragraph::Paragraph(std::vector<std::shared_ptr<inline_layout::Span>> content,
                     inline_layout::LineMetrics metrics,
                     bool is_repaint_boundary)
    : content(content),
      metrics(metrics),
      Element(is_repaint_boundary, /* size_depends_on_parent */ true){};

void Paragraph::next_line() {
    current_line = &lines.emplace_back(this);
    remaining_width = total_width;
};

Size Paragraph::layout(BoxConstraints constraints) {
    // Layout spans into lines
    total_width = constraints.max_width;
    lines.clear();
    next_line();
    for (auto& span : content) layout_span(span.get());
 
    // Render spans in lines
    current_height = 0;
    for (auto& line : lines) {
        current_height += line.render(Position{0, current_height});
    }

    return Size{constraints.max_width, current_height};
};

void Paragraph::layout_span(inline_layout::Span* span) {
    auto constraints = inline_layout::InlineConstraints{
        remaining_width,  // remaining_line_width
        total_width,      // total_line_width
        0,                // padding_before
        0                 // padding_after
    };
    auto result = span->layout(constraints);
    if (result.fit_span != std::nullopt) {
        auto fit_span = result.fit_span.value();
        fit_span->metrics = result.metrics;
        fit_span->width = result.width;
        current_line->spans.push_back(fit_span);
        remaining_width -= result.width;
    }
    if (result.remainder_span != std::nullopt) {
        next_line();
        layout_span(result.remainder_span.value());
    }
};

void Paragraph::paint(bool is_changed) {
    for (auto& line : lines) {
        for (auto& elem : line.elements) {
            document->paint_element(elem.get());
        }
    }
};

}  // namespace aardvark::elements
