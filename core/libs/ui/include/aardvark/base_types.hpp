#pragma once

#include <array>
#include <nod/nod.hpp>

#include "SkColor.h"
#include "SkMatrix.h"

namespace aardvark {

class Connection {
  public:
    virtual void disconnect(){};
};

class NodConnection : public Connection {
  public:
    NodConnection(nod::connection conn) : conn(std::move(conn)){};
    void disconnect() override { conn.disconnect(); };

  private:
    nod::connection conn;
};

struct Value {
    enum class ValueType { none, abs, rel };

    Value() = default;
    Value(ValueType type, float value) : type(type), value(value){};

    ValueType type = ValueType::none;
    float value = 0;

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

inline bool operator==(const Value& lhs, const Value& rhs) {
    return lhs.type == rhs.type && lhs.value == rhs.value;
}

struct Scale {
    float horiz = 1;
    float vert = 1;
};

struct Size {
    float width = 0;
    float height = 0;

    Size scale(float ratio) {
      return Size{width * ratio, height * ratio};
    };

    static bool is_equal(Size a, Size b);
};

inline bool operator==(const Size& lhs, const Size& rhs) {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}

inline bool operator!=(const Size& lhs, const Size& rhs) {
    return !(lhs == rhs);
}

struct Position {
    Position() = default;
    Position(float left, float top) : left(left), top(top){};
    float left = 0;
    float top = 0;
    static Position add(Position a, Position b);
    static Position origin;
};

inline bool operator==(const Position& lhs, const Position& rhs) {
    return lhs.left == rhs.left && lhs.top == rhs.top;
}

inline bool operator!=(const Position& lhs, const Position& rhs) {
    return !(lhs == rhs);
}

inline Position operator+(const Position& lhs, const Position& rhs) {
    return Position(
        lhs.left + rhs.left,  // left
        lhs.top + rhs.top     // top
    );
}

struct Color {
    int red = 0;
    int green = 0;
    int blue = 0;
    int alpha = 0;

    SkColor to_sk_color() const {
        return SkColorSetARGB(alpha, red, green, blue);
    }

    static Color from_sk_color(const SkColor& sk_color) {
        return Color{
            static_cast<int>(SkColorGetR(sk_color)),
            static_cast<int>(SkColorGetG(sk_color)),
            static_cast<int>(SkColorGetB(sk_color)),
            static_cast<int>(SkColorGetA(sk_color))};
    }

    static Color black;
};

inline bool operator==(const Color& lhs, const Color& rhs) {
    return memcmp(&lhs, &rhs, sizeof(Color)) == 0;
}

inline bool operator!=(const Color& lhs, const Color& rhs) {
    return !(lhs == rhs);
}

struct Transform {
    std::array<float, 9> values = {1, 0, 0, 0, 1, 0, 0, 0, 1};  // identity

    SkMatrix to_sk_matrix() const {
        auto matrix = SkMatrix();
        matrix.set9(values.data());
        return matrix;
    }

    static Transform from_sk_matrix(const SkMatrix& matrix) {
        auto transform = Transform();
        matrix.get9(transform.values.data());
        return transform;
    }
};

}  // namespace aardvark
