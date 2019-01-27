#pragma once

#include <optional>
#include <memory>
#include "../element.hpp"
#include "../elements/border.hpp"
#include "../elements/background.hpp"
#include "span.hpp"
#include "utils.hpp"

namespace aardvark::inline_layout {

struct Decoration {
    std::optional<SkColor> background;
    std::optional<elements::BoxBorders> borders;
    std::optional<elements::EdgeInsets> insets;

    // Splits decoration into left and right parts
    std::pair<Decoration, Decoration> split();
 
    // Calculates widths of left and right paddings
    std::pair<float, float> get_paddings(float total_line_width);
};

class DecorationSpan : public Span {
  public:
    DecorationSpan(std::vector<std::shared_ptr<Span>> content,
                   Decoration decoration,
                   std::optional<SpanBase> base_span = std::nullopt);
    InlineLayoutResult layout(InlineConstraints constraints) override;
    std::shared_ptr<Element> render(
        std::optional<SpanSelectionRange> selection) override;
    std::vector<std::shared_ptr<Span>> content;
    Decoration decoration;
};

}  // namespace aardvark::inline_layout

