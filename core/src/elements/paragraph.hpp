#pragma once

#include "../base_types.hpp"
#include "../element.hpp"
#include "../inline_layout/span.hpp"
#include "../inline_layout/utils.hpp"

namespace aardvark::elements {

struct Selection {
    inline_layout::Span* base;
    int base_offset;
    inline_layout::Span* extent;
    int extent_offset;
};

using ParagraphLine = std::vector<std::shared_ptr<inline_layout::Span>>;

class Paragraph : public Element {
  public:
    Paragraph(std::vector<std::shared_ptr<inline_layout::Span>> content,
              inline_layout::LineMetrics metrics,
              bool is_repaint_boundary = false);
    std::vector<std::shared_ptr<inline_layout::Span>> content;
    std::string get_debug_name() override { return "Paragraph"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;

  private:
    inline_layout::LineMetrics metrics;
    void next_line();
    void layout_span(std::shared_ptr<inline_layout::Span> span_sp);
    std::vector<ParagraphLine> lines;
    ParagraphLine* current_line;
    std::vector<std::shared_ptr<Element>> elements;
    float current_height;
    float total_width;
    float remaining_width;
};

}  // namespace aardvark::elements
