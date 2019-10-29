#include "clip.hpp"

namespace aardvark::elements {

Size Clip::layout(BoxConstraints constraints) {
    child->size =
        document->layout_element(child.get(), constraints.make_loose());
    child->rel_position = Position{0, 0};
    return constraints.max_size();
}

void Clip::paint(bool is_changed) {
    child->clip = clipper(size);
    document->paint_element(child.get());
}

SkPath Clip::default_clip(Size size) {
    SkPath path;
    path.addRect(0, 0, size.width, size.height);
    return path;
}

}  // namespace aardvark::elements
