#include "Color.h"

Color::Color(float a,float r,float g,float b)
{
    _a = a;
    _r = r;
    _g = g;
    _b = b;
}

float Color::a()           { return _a; }
void Color::a(float  value){ _a = value; }

float Color::r()           { return _r; }
void Color::r(float  value){ _r = value; }

float Color::g()           { return _g; }
void Color::g(float  value){ _g = value; }

float Color::b()           { return _b; }
void Color::b(float  value){ _b = value; }

bool Color::isFull()
{
	return _a == FULL_COLOR && _r == FULL_COLOR && _g == FULL_COLOR && _b == FULL_COLOR;
}
