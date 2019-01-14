#include <memory>
#include <optional>
#include <variant>
#include <unicode/unistr.h>
#include "../base_types.hpp"
#include "SkPaint.h"

namespace aardvark::inline_layout {

// Measures text width, paint must have UTF16 encoding.
int measure_text(const UnicodeString& text, const SkPaint& paint);

// Convert ICU string to C++ UTF-8 encoded string.
std::string to_std_string(const UnicodeString& text);

enum class LineBreak {
    // Use default unicode line breaking algorithm
    normal,

    // Never allow break text
    never,

    // Line break is allowed between any two characters
    anywhere,

    // If word can not fit in the line using default breaking algorithm, it is
    // allowed to break it at arbitrary point
    overflow
};

class Span;

struct InlineConstraints {
    int remaining_line_width;
    int total_line_width;
    int padding_before;
    int padding_after;
};

struct InlineLayoutResult {
    std::optional<Span*> fit_span;
    Size size;
    int baseline;
    std::optional<Span*> remainder_span;
};

// Representation of the selected range in the inline container element
struct Selection {
    Span* base;
    int base_offset;
    Span* extent;
    int extent_offset;
};

struct start {};
struct end {};
struct offset {
    offset(int pos) : pos(pos){};
    int pos;
};

// Selected range in the span object
using SpanSelection = std::variant<start, end, offset>;

struct SpanBase {
    Span* span;
    int prev_offset;
};

// Span represents part of the contents of the inline container element
class Span {
  public:
    Span(std::optional<SpanBase> base_span = std::nullopt){};

    // Defines how the content should be lay out by the container
    virtual InlineLayoutResult layout(InlineConstraints constraints){};

    // Returns elements that represent this span in the document
    // virtual std::shared_ptr<Element> render(
    // std::optional<SpanSelection> selection
    // ) {};

    // virtual ResponderMode get_responder_mode();
    // virtual Responder* get_responder() { return nullptr; };

    // Span from which this span is derived
    std::optional<SpanBase> base_span;

    // Should be set by container during layout
    Size size;
    int line_height;
    int line_baseline;
};

/*
class VerticalAlignSpan : public Span {
    std::shared_ptr<Span> content;
    VerticalAlign align;
};
*/

/*
struct TextStyle {
    int font_face;
    int font_size;
    int font_style;
    int font_weight;
    int line_height;
    int color;
    int letter_spacing;
    // int word_spacing;
}
*/

class SpacingSpan : public Span {
  public:
    SpacingSpan(int width, std::optional<SpanBase> base_span = std::nullopt)
        : width(width), Span(base_span){};
    int width;
};

/*
class DecorationSpan : public Span {
  public:
    std::vector<std::shared_ptr<Span>> content;
    int padding;
    int margin;
    int border;
    int background;
};

class ElementSpan : public Span {
  public:
    Size size;
    std::shared_ptr<Element> element;
};
*/

};  // namespace aardvark::inline_layout
