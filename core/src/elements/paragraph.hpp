#include "../base_types.hpp"
#include "../element.hpp"
#include "../inline_layout/inline_layout.hpp"

namespace aardvark::elements {

class ParagraphLine {
  public:
    int render(Position position);
    std::vector<Span*> spans;
    std::vector<std::shared_ptr<Element>> elements;
    int height = 0;
};

class Paragraph : public Element {
  public:
    Paragraph(std::vector<Span> content, is_repaint_boundary = false);
    std::vector<Span> content;
    std::string get_debug_name() override { return "Paragraph"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;

  private:
    std::vector<ParagraphLine> lines;
    ParagraphLine& current_line;
    int current_height;
    int total_width;
    int remaining_width;
};

}  // namespace aardvark::elements
