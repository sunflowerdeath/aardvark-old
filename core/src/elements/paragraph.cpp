#include "../element.hpp"

namespace aardvark::elements {

int ParagraphLine::render(Position position) {
    elements.reserve(spans.size);
    for (auto& span : spans) {
        // Height of the line is equal to the height of the tallest span
        if (span->size.height > height) height = span->size.height;
    }
    auto current_width = 0;
    for (auto& span : spans) {
        auto elem = span->render(/* selection */ std::nullopt);
        elem->parent = paragraph;
        auto constraints = Constraints::from_size(span.size, /* tight */ true);
        auto size = paragraph->document->layout_element(elem, constraints);
        elem->size = size;
        elem->rel_position = position + Position{current_width, 0};
        current_width += span.size.width;
        elements.push_back(elem);
    }
    return height;
};

Paragraph::Paragraph(std::vector<std::shared_ptr<Span>> content,
                     is_repaint_boundary = false)
    : content(content),
      Element(is_repaint_boundary, /* size_depends_on_parent */ true){};

void Paragraph::next_line() {
    current_line = lines.emplace_back();
    remaining_width = total_width;
};

Size Paragraph::layout(BoxConstraints constraints) {
    // Layout spans into lines
    total_width = constraints.width;
    lines.clear()
    next_line();
    for (auto& span : content) layout_span(span.get());
 
    // Render spans in lines
    current_height = 0;
    for (auto& line : lines) {
        current_height +=
            line.render(current_height, /* selection */ std::nullopt);
    }
};

void Paragraph::layout_span(const Span& span) {
    auto constraints = InlineConstraints{
        remaining_width,    // remaining_line_width
        constraints.width,  // total_line_width
        0,                  // padding_before
        0                   // padding_after
    };
    auto result = span.layout(constraints);
    if (result.fit_span != std::nullopt) {
        remaining_width -= result.size.width;
        fit_span->size = result->size;
        fit_span->baseline = result->baseline;
        current_line.spans.push_back(span);
    }
    if (result.remainder_span != std::nullopt) {
        next_line();
        layout_span(result.remainder_span);
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
