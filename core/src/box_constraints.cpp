#include "box_constraints.hpp"

namespace aardvark {

BoxConstraints BoxConstraints::make_loose() {
    return BoxConstraints{
        0,          // min_width
        max_width,  // max_width
        0,          // min_height
        max_height  // max_height
    };
};

bool BoxConstraints::is_tight() {
    return min_width == max_width && min_height == max_height;
};

Size BoxConstraints::max_size() { return Size{max_width, max_height}; }

BoxConstraints BoxConstraints::from_size(Size size, bool tight) {
    return BoxConstraints{
        tight ? size.width : 0,   // min_width
        size.width,               // max_width
        tight ? size.height : 0,  // min_height
        size.height               // max_height
    };
};

}  // namespace aardvark
