#include "../base_types.hpp"
#include "../element.hpp"
#include "../inline_layout/inline_layout.hpp"

namespace aardvark::elements {

class Paragraph;

class ParagraphLine {
  public:
    ParagraphLine(Paragraph* paragraph) : paragraph(paragraph){};
    float render(Position position);
    Paragraph* paragraph;
    std::vector<inline_layout::Span*> spans;
    std::vector<std::shared_ptr<Element>> elements;
};

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
    void layout_span(inline_layout::Span* span);
    std::vector<ParagraphLine> lines;
    ParagraphLine* current_line;
    float current_height;
    float total_width;
    float remaining_width;
};

}  // namespace aardvark::elements
