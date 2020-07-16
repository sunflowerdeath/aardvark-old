#pragma once

#include "../base_types.hpp"
#include "../element.hpp"
#include "../inline_layout/span.hpp"
#include "../inline_layout/utils.hpp"
#include "../inline_layout/decoration_span.hpp"

namespace aardvark {

struct Selection {
    inline_layout::Span* base;
    int base_offset;
    inline_layout::Span* extent;
    int extent_offset;
};

using ParagraphLine = std::vector<std::shared_ptr<inline_layout::Span>>;

class ParagraphElement : public Element {
  public:
    ParagraphElement()
        : root(std::make_shared<inline_layout::DecorationSpan>()),
          metrics(inline_layout::LineMetrics::from_font(
                      inline_layout::make_default_font())
                      .scale(1.5)),
          Element(
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ false){};

    ParagraphElement(
        // std::vector<std::shared_ptr<inline_layout::Span>> children,
        std::shared_ptr<inline_layout::Span> root,
        inline_layout::LineMetrics metrics,
        bool is_repaint_boundary = false);

    std::vector<std::shared_ptr<inline_layout::Span>> content;
    std::string get_debug_name() override { return "Paragraph"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
    void visit_children(ChildrenVisitor visitor) override;
    float get_intrinsic_height(float width) override;
    float get_intrinsic_width(float height) override;
    // TODO intrinsic

    std::shared_ptr<inline_layout::Span> root;
    inline_layout::LineMetrics metrics;

  private:
    void next_line();
    float layout_inline(float max_width);
    void layout_span(std::shared_ptr<inline_layout::Span> span_sp);
    std::vector<ParagraphLine> lines;
    ParagraphLine* current_line;
    std::vector<std::shared_ptr<Element>> elements;
    float total_width;
    float remaining_width;
};

}  // namespace aardvark::elements
