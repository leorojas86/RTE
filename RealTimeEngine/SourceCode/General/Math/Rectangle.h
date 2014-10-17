/*
 *  Rectangle.h
 *  MyTestIPhoneApp
 *
 *  Created by Leonardo Rojas on 12/2/09.
 *  Copyright 2009 FPL. All rights reserved.
 *
 */

#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Circle.h"
#include <string>

class Rectangle
{
public:
    explicit Rectangle();
    explicit Rectangle(Point3D location, Size3D size);

    Point3D& location();
    void location(Point3D location);
    
    Size3D& size();
    void size(Size3D size);
    
    Size3D getIntersection(Rectangle& rectangle);
    Size3D getIntersection(Circle& circle);
    
    Circle getBoundingCircle();
    bool contains(Point3D& point);
    
    float getMaxX();
    float getMinX();
    float getMaxY();
    float getMinY();

    static std::string toString(Rectangle& rectangle);
    static Rectangle fromString(std::string& value);

protected:
    Point3D _location;
    Size3D _size;
};

#endif
