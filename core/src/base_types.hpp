#pragma once
namespace aardvark {

namespace value {

struct rel {
  rel(float val): val(val) {};
  float val;
};

struct abs {
  abs(int val): val(val) {};
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
  inline bool operator==(const Size& rhs) {
    return width == rhs.width && height == rhs.height;
  };
};

struct Position {
	int left = 0;
	int top = 0;
  inline bool operator==(const Position& rhs) {
    return left == rhs.left && top == rhs.top;
  };
	static Position add(Position a, Position b);
};

};

