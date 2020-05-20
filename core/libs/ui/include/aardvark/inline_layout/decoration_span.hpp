#pragma once

#include <memory>
#include <optional>

#include "../base_types.hpp"
#include "../element.hpp"
#include "../node.hpp"
#include "../elements/background.hpp"
#include "../elements/border.hpp"
#include "../elements/padding.hpp"
#include "../elements/stack.hpp"
#include "span.hpp"
#include "utils.hpp"

namespace aardvark::inline_layout {

struct Decoration {
    std::optional<Color> background = std::nullopt;
    std::optional<BoxBorders> borders = std::nullopt;
    std::optional<Padding> padding = std::nullopt;

    // Splits decoration into left and right parts
    Decoration left();
    Decoration right();
 
    // Calculates widths of left and right paddings
    std::pair<float, float> get_paddings(float total_line_width);
};

class DecorationSpan : public MultipleChildrenSpan {
  public:
    DecorationSpan(
        std::vector<std::shared_ptr<Span>> children = {},
        Decoration decoration = Decoration(),
        std::optional<SpanBase> base_span = std::nullopt);

    InlineLayoutResult layout(InlineConstraints constraints) override;
    std::shared_ptr<Element> render() override;
    UnicodeString get_text() override;
    int get_text_length() override;
    std::shared_ptr<Span> slice(int start, int end) override;
    int get_text_offset_at_position(int position) override;

    Decoration decoration;
};

}  // namespace aardvark::inline_layout

