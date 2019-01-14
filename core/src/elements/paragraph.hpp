#include "../base_types.hpp"
#include "../element.hpp"
#include "../inline_layout/inline_layout.hpp"

namespace aardvark::elements {

class Paragraph;

class ParagraphLine {
  public:
    int render(Position position);
    Paragraph* paragraph;
    std::vector<inline_layout::Span*> spans;
    std::vector<std::shared_ptr<Element>> elements;
    int height = 0;
};

class Paragraph : public Element {
  public:
    Paragraph(std::vector<std::shared_ptr<inline_layout::Span>> content,
              bool is_repaint_boundary = false);
    std::vector<std::shared_ptr<inline_layout::Span>> content;
    std::string get_debug_name() override { return "Paragraph"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;

  private:
    void next_line();
    void layout_span(inline_layout::Span* span);
    std::vector<ParagraphLine> lines;
    ParagraphLine* current_line;
    int current_height;
    int total_width;
    int remaining_width;
};

}  // namespace aardvark::elements
