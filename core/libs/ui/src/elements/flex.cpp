#include "elements/flex.hpp"

namespace aardvark {

float FlexElement::get_intrinsic_height(float width) {
    auto result = 0.0;
    for (auto& child : children) {
        if (direction == FlexDirection::column) {
            // TODO probably not correct
            result += child->get_intrinsic_height(width);
        } else {
            result = fmax(result, child->get_intrinsic_height(width));
        }
    }
    return result;
}

float FlexElement::get_intrinsic_width(float height) {
    auto result = 0.0;
    for (auto& child : children) {
        if (direction == FlexDirection::row) {
            // TODO probably not correct
            result += child->get_intrinsic_width(height);
        } else {
            result = fmax(result, child->get_intrinsic_width(height));
        }
    }
    return result;
}

inline float get_main(Size size, FlexDirection direction) {
    return direction == FlexDirection::row ? size.width : size.height;
}

inline float get_cross(Size size, FlexDirection direction) {
    return direction == FlexDirection::row ? size.height : size.width;
}

inline BoxConstraints make_constraints(
    FlexDirection direction,
    float min_main,
    float max_main,
    float min_cross,
    float max_cross) {
    return direction == FlexDirection::row
               ? BoxConstraints{min_main, max_main, min_cross, max_cross}
               : BoxConstraints{min_cross, max_cross, min_main, max_main};
}

inline FlexAlign get_align(Element* elem, FlexAlign align) {
    auto flex_child = dynamic_cast<FlexChildElement*>(elem);
    return (flex_child == nullptr || flex_child->align == std::nullopt)
               ? align
               : flex_child->align.value();
}

Size FlexElement::layout(BoxConstraints constraints) {
    auto fixed_children = std::vector<Element*>();
    auto flex_children = std::vector<FlexChildElement*>();
    auto total_flex = 0;
    for (auto& child : children) {
        auto flex_child = dynamic_cast<FlexChildElement*>(child.get());
        if (flex_child != nullptr && flex_child->flex > 0) {
            flex_children.push_back(flex_child);
            total_flex += flex_child->flex;
        } else {
            fixed_children.push_back(child.get());
        }
    }

    // Here `main` and `cross` used instead of `width` and `height`,
    // and `main_pos` and `cross_pos` instead of `left` and `top`.
    auto max_main = direction == FlexDirection::row ? constraints.max_width
                                                    : constraints.max_height;
    auto min_cross = direction == FlexDirection::row ? constraints.min_height
                                                     : constraints.min_width;
    auto max_cross = direction == FlexDirection::row ? constraints.max_height
                                                     : constraints.max_width;

    auto remaining_main = max_main;
    auto max_child_cross = 0.0f;

    // Layout fixed children
    for (auto child : fixed_children) {
        auto child_min_cross =
            get_align(child, align) == FlexAlign::stretch ? max_cross : 0;
        auto child_constraints = make_constraints(
            direction, 0, remaining_main, child_min_cross, max_cross);
        auto size = document->layout_element(child, child_constraints);
        max_child_cross = fmax(get_cross(size, direction), max_child_cross);
        child->size = size;
        remaining_main -= get_main(size, direction);
    }

    // Layout flexible children and distribute remaining size between them
    if (remaining_main < 0) remaining_main = 0;
    auto single_flex = remaining_main / total_flex;
    for (auto child : flex_children) {
        auto child_main = single_flex * static_cast<float>(child->flex);
        auto child_min_cross =
            get_align(child, align) == FlexAlign::stretch ? max_cross : 0;
        auto child_constraints = make_constraints(
            direction, 0, child_main, child_min_cross, max_cross);
        auto size = document->layout_element(child, child_constraints);
        max_child_cross = fmax(get_cross(size, direction), max_child_cross);
        child->size = size;
    }

    auto cross = fmax(max_child_cross, min_cross);

    // Position children and distibute remaining size according to justify
    auto current_main_pos = 0.0f;
    auto space_before = 0.0f;
    auto space_after = 0.0f;
    switch (justify) {
        case FlexJustify::start:
            // do nothing
            break;
        case FlexJustify::center:
            current_main_pos = remaining_main / 2;
            break;
        case FlexJustify::end:
            current_main_pos = remaining_main;
            break;
        case FlexJustify::space_around:
            space_before = remaining_main / children.size() / 2;
            space_after = space_before;
            break;
        case FlexJustify::space_between:
            space_after = remaining_main / (children.size() - 1);
            break;
        case FlexJustify::space_evenly:
            space_before = remaining_main / (children.size() + 1);
            break;
    }
    for (auto& child : children) {
        current_main_pos += space_before;
        auto child_align = get_align(child.get(), align);
        auto child_cross_pos = 0.0f;
        if (child_align == FlexAlign::center) {
            child_cross_pos = (cross - get_cross(child->size, direction)) / 2;
        } else if (child_align == FlexAlign::end) {
            child_cross_pos = cross - get_cross(child->size, direction);
        }
        child->rel_position = direction == FlexDirection::row
                                  ? Position{current_main_pos, child_cross_pos}
                                  : Position{child_cross_pos, current_main_pos};
        current_main_pos += get_main(child->size, direction) + space_after;
    }

    return direction == FlexDirection::row ? Size{max_main, cross}
                                           : Size{cross, max_main};
}

Size FlexChildElement::layout(BoxConstraints constraints) {
    auto child_constraints =
        (flex > 0 && tight_fit) ? constraints.make_tight() : constraints;
    auto size = document->layout_element(child.get(), child_constraints);
    child->size = size;
    child->rel_position = Position{0, 0};
    return size;
}

}  // namespace aardvark
