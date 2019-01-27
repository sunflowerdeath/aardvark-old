#include "decoration_span.hpp"

namespace aardvark::inline_layout {

std::pair<Decoration, Decoration> Decoration::split() {
    std::optional<elements::BoxBorders> left_borders = std::nullopt; 
    std::optional<elements::BoxBorders> right_borders = std::nullopt; 
    if (borders != std::nullopt) {
       left_borders = borders.value();
       right_borders = borders.value();
       left_borders.value().right = elements::BorderSide::none();
       right_borders.value().left = elements::BorderSide::none();
    }

    std::optional<elements::EdgeInsets> left_insets = std::nullopt;
    std::optional<elements::EdgeInsets> right_insets = std::nullopt;
    if (insets != std::nullopt) {
       left_insets = insets.value();
       right_insets = insets.value();
       left_insets.value().right = Value::none();
       left_insets.value().left = Value::none();
    }

    return std::make_pair(Decoration{background, left_borders, left_insets},
                          Decoration{background, right_borders, right_insets});
};

std::pair<float, float> Decoration::get_paddings(float total_line_width) {
    auto before = 0;
    auto after = 0;
    if (insets != std::nullopt) {
        before += insets.value().left.calc(total_line_width);
        after += insets.value().right.calc(total_line_width);
    }
    if (borders != std::nullopt) {
        before += borders.value().left.width;
        after += borders.value().right.width;
    }
    return std::make_pair(before, after);
};

DecorationSpan::DecorationSpan(std::vector<std::shared_ptr<Span>> content,
                               Decoration decoration,
                               std::optional<SpanBase> base_span)
    : content(content), decoration(decoration), Span(base_span){};

InlineLayoutResult DecorationSpan::layout(InlineConstraints constraints) {
    std::vector<std::shared_ptr<Span>> fit_spans;
    std::vector<std::shared_ptr<Span>> remaining_spans;
    auto fit_spans_width = 0;
    auto reached_end = false;
    auto paddings = decoration.get_paddings(constraints.total_line_width);
    for (auto& span : content) {
        if (reached_end) {
            remaining_spans.push_back(span);
            continue;
        }

        auto span_constraints = InlineConstraints{
            constraints.remaining_line_width -
                fit_spans_width,                 // remaining_line_width
            constraints.total_line_width,  // total_line_width
            span == content.front() ? std::get<0>(paddings)
                                    : 0,  // padding_before
            span == content.back() ? std::get<1>(paddings) : 0  // padding_after
        };
        auto result = Span::layout(span, span_constraints);
        if (result.fit_span != std::nullopt) {
            auto fit_span = result.fit_span.value();
            fit_span->metrics = result.metrics;
            fit_span->width = result.width;
            fit_spans.push_back(fit_span);
            fit_spans_width += result.width;
        }
        if (result.remainder_span != std::nullopt) {
            reached_end = true;
            remaining_spans.push_back(result.remainder_span.value());
        }
    }

    if (remaining_spans.size() == 0) {
        // TODO use default_metrics?
        auto fit_span_metrics =
            calc_combined_metrics(fit_spans, LineMetrics{0, 0, 0});
        return InlineLayoutResult::fit(fit_spans_width, fit_span_metrics);
    } else if (fit_spans.size() == 0) {
        return InlineLayoutResult::wrap();
    } else {
        auto split_decoration = decoration.split();
        auto fit_span = std::make_shared<DecorationSpan>(
            fit_spans,                      // content
            std::get<0>(split_decoration),  // decoration
            SpanBase{this, 0}               // base_span
        );
        // TODO use default_metrics?
        auto fit_span_metrics =
            calc_combined_metrics(fit_spans, LineMetrics{0, 0, 0});
        auto remainder_span = std::make_shared<DecorationSpan>(
            remaining_spans,                                    // content
            std::get<1>(split_decoration),                      // decoration
            SpanBase{this, static_cast<int>(fit_spans.size())}  // base_span
        );
        return InlineLayoutResult::split(
            constraints.remaining_line_width,  // width
            fit_span_metrics,                  // metrics
            fit_span,                          // fit_span
            remainder_span                     // remainder_span
        );
    }
};

std::shared_ptr<Element> DecorationSpan::render(
    std::optional<SpanSelectionRange> selection) {
    auto stack = std::make_shared<elements::Stack>();
    if (decoration.background != std::nullopt) {
        auto background = std::make_shared<elements::Background>(
            decoration.background.value());
        stack->children.push_back(background);
    }
    render_spans(content, metrics, Position{0, 0}, &stack->children);

    std::shared_ptr<Element> container = stack;
    auto align = 0;
    if (decoration.insets != std::nullopt) {
        auto insets = decoration.insets.value();
        container = std::make_shared<elements::Align>(container, insets);
        align += insets.top.calc(metrics.height);
    }
    if (decoration.borders != std::nullopt) {
        auto borders = decoration.borders.value();
        container = std::make_shared<elements::Border>(
            container, borders,
            elements::BoxRadiuses::all(elements::Radius{0, 0}));
        align += borders.top.width;
    }
    if (align > 0) {
        container = std::make_shared<elements::Align>(
            container, elements::EdgeInsets{Value::abs(0), Value::abs(-align)});
    }
    return container;
};

}  // namespace aardvark::inline_layout
