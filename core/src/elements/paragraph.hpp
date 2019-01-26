#pragma once

#include "../base_types.hpp"
#include "../element.hpp"
#include "../inline_layout/inline_layout.hpp"
#include "../inline_layout/utils.hpp"

namespace aardvark::elements {

using ParagraphLine = std::vector<std::shared_ptr<inline_layout::Span>>;

class Paragraph : public Element {
    friend ParagraphLine;

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
