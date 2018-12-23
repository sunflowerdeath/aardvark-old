#pragma once
namespace aardvark {

namespace value {

struct rel {
    rel(float val) : val(val){};
    float val;
};

struct abs {
    abs(int val) : val(val){};
    int val;
};

struct none {};

}  // namespace value

struct Scale {
    float horiz = 1;
    float vert = 1;
};

struct Size {
    int width = 0;
    int height = 0;
    static bool is_equal(Size a, Size b);
};

inline bool operator==(const Size& lhs, const Size& rhs) {
    return lhs.width == rhs.width && lhs.height == rhs.height;
};

inline bool operator!=(const Size& lhs, const Size& rhs) {
    return !(lhs == rhs);
};

struct Position {
    int left = 0;
    int top = 0;
    static Position add(Position a, Position b);
};

inline bool operator==(const Position& lhs, const Position& rhs) {
    return lhs.left == rhs.left && lhs.top == rhs.top;
};

inline bool operator!=(const Position& lhs, const Position& rhs) {
    return !(lhs == rhs);
};

inline Position operator+(const Position& lhs, const Position& rhs) {
    return Position{
        lhs.left + rhs.left,  // left
        lhs.top + rhs.top     // top
    };
};

};  // namespace aardvark

