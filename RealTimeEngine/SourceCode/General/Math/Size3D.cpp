/*
 *  Size2D.cpp
 *  MyTestIPhoneApp
 *
 *  Created by Leonardo Rojas on 12/2/09.
 *  Copyright 2009 FPL. All rights reserved.
 *
 */

#include "Size3D.h"

Size3D::Size3D(float width, float height, float depth)
{
    w = width;
    h = height;
    d = depth;
}

bool Size3D::isEmpty()
{
    return w == 0 && h == 0 && d == 0;
}

float Size3D::aspecRatio()
{
    return w / h;
}

void Size3D::multiply(Size3D& size)
{
    w *= size.w;
    h *= size.h;
    d *= size.d;
}

bool Size3D::equals(Size3D& size, Size3D& other)
{
    return size.w == other.w && size.h == other.h && size.d == other.d;
}
