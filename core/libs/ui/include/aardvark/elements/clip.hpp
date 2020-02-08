#pragma once

#include <memory>
#include <functional>
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../element.hpp"
#include "SkPath.h"

namespace aardvark::elements {

using Clipper = std::function<SkPath(Size)>;

class Clip : public SingleChildElement {
  public:
    // TODO default clip
    Clip()
        : SingleChildElement(nullptr, /* is_repaint_boundary */ false,
                             /* size_depends_on_parent */ true){};

    Clip(std::shared_ptr<Element> child, Clipper clipper)
        : SingleChildElement(child, /* is_repaint_boundary */ false,
                             /* size_depends_on_parent */ true),
          clipper(clipper){};

    Clipper clipper = &Clip::default_clip;
    Size layout(BoxConstraints constraints) override;
    void paint(bool is_changed) override;

    static SkPath default_clip(Size size);
};

}  // namespace aardvark::elements
