#ifndef COLOR_H
#define COLOR_H

#define EMPTY_COLOR 0
#define FULL_COLOR  1

class Color
{
public:
    explicit Color(float a = FULL_COLOR,float r = FULL_COLOR,float g = FULL_COLOR,float b = FULL_COLOR);

    float a();
    float r();
    float g();
    float b();
    void a(float  value);
    void r(float  value);
    void g(float  value);
    void b(float  value);

    bool isFull();

protected:
    float _a;
    float _r;
    float _g;
    float _b;
};

#endif
