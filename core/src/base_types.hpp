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
};

struct Position {
	int left = 0;
	int top = 0;
	static Position add(Position a, Position b);
};

};

