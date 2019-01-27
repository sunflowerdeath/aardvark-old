#pragma once

#include <memory>
#include <unicode/unistr.h>
#include "SkPaint.h"
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"
#include "../inline_layout/line_metrics.hpp"

namespace aardvark::elements {

class Text : public Element {
  public:
    Text(UnicodeString text, SkPaint paint, bool is_repaint_boundary = false);
    std::string get_debug_name() override { return "Text"; };
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;
    UnicodeString text;
    SkPaint skpaint;
};

}  // namespace aardvark::elements
