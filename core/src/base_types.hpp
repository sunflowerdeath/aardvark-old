#pragma once
namespace aardvark {

struct Scale {
	float horiz = 1;
	float vert = 1;
};

struct Size {
	float width = 0;
	float height = 0;
	static bool is_equal(Size a, Size b);
};

struct Position {
	float left = 0;
	float top = 0;
	static Position add(Position a, Position b);
};

};

