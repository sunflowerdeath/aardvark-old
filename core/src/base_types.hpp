#pragma once

namespace aardvark {

struct Value {
    enum class ValueType { none, abs, rel };
    ValueType type;
    float value;

    Value(ValueType type, float value) : type(type), value(value){};

    float calc(float total, float fallback = 0) {
        if (type == ValueType::none) return fallback;
        if (type == ValueType::abs) return value;
        return value * total;  // rel
    };

    bool is_none() const { return type == ValueType::none; };

    static Value abs(float value) { return Value(ValueType::abs, value); };
    static Value rel(float value) { return Value(ValueType::rel, value); };
    static Value none() { return Value(ValueType::none, 0); };
};

struct Scale {
    float horiz = 1;
    float vert = 1;
};

struct Size {
    float width = 0;
    float height = 0;
    static bool is_equal(Size a, Size b);
};

inline bool operator==(const Size& lhs, const Size& rhs) {
    return lhs.width == rhs.width && lhs.height == rhs.height;
};

inline bool operator!=(const Size& lhs, const Size& rhs) {
    return !(lhs == rhs);
};

struct Position {
    Position(float left = 0, float top = 0) : left(left), top(top) {};
    float left = 0;
    float top = 0;
    static Position add(Position a, Position b);
};

inline bool operator==(const Position& lhs, const Position& rhs) {
    return lhs.left == rhs.left && lhs.top == rhs.top;
};

inline bool operator!=(const Position& lhs, const Position& rhs) {
    return !(lhs == rhs);
};

inline Position operator+(const Position& lhs, const Position& rhs) {
    return Position(
        lhs.left + rhs.left,  // left
        lhs.top + rhs.top     // top
    );
};

};  // namespace aardvark

