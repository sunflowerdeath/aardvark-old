#pragma once
namespace aardvark {

struct Scale {
	float horiz = 1;
	float vert = 1;
};

struct Size {
	float width;
	float height;
	static bool isEqual(Size a, Size b);
};

struct Position {
	float left;
	float top;
	static Position add(Position a, Position b);
};

};

