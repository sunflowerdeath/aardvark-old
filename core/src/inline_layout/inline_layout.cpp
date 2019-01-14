#include "inline_layout.hpp"

namespace aardvark::inline_layout {

int measure_text(const UnicodeString& text, const SkPaint& paint) {
    return paint.measureText(text.getBuffer(), text.length() * 2);
};

std::string to_std_string(const UnicodeString& text) {
    std::string std_string;
    text.toUTF8String(std_string);
    return std_string;
};

} // namespace aardvark::inline_layout
