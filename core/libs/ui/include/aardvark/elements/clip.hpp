#pragma once

#include <functional>

#include "../base_types.hpp"
#include "../element.hpp"
#include "SkPath.h"

namespace aardvark {

using Clipper = std::function<SkPath(Size)>;

class ClipElement : public SingleChildElement {
  public:
    ClipElement()
        : SingleChildElement(
              nullptr,
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ true){};

    ClipElement(std::shared_ptr<Element> child, Clipper clipper)
        : SingleChildElement(
              std::move(child),
              /* is_repaint_boundary */ false,
              /* size_depends_on_parent */ true),
          clipper(std::move(clipper)){};

    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;

    Clipper clipper = &ClipElement::default_clip;

    static SkPath default_clip(Size size);
};

}  // namespace aardvark
