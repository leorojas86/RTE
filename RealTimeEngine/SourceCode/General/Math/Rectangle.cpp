/*
 *  Rectangle.cpp
 *  MyTestIPhoneApp
 *
 *  Created by Leonardo Rojas on 12/2/09.
 *  Copyright 2009 FPL. All rights reserved.
 *
 */

#include "Rectangle.h"
#include <math.h>

Rectangle::Rectangle()
{
}

Rectangle::Rectangle(Point3D location, Size3D size)
{
    _location = location;
    _size     = size;
}

Point3D& Rectangle::location()              { return _location; }
void Rectangle::location(Point3D location)  { _location = location; }

Size3D& Rectangle::size()           { return _size; }
void Rectangle::size(Size3D size)   { _size = size; }

Size3D Rectangle::getIntersection(Rectangle& rectangle)
{
    float xIntersection = fminf(getMaxX(),rectangle.getMaxX()) - fmaxf(getMinX(),rectangle.getMinX());
    
    if(xIntersection > 0)
    {
        float yIntersection = fminf(getMaxY(),rectangle.getMaxY()) - fmaxf(getMinY(),rectangle.getMinY());
        return Size3D(xIntersection,yIntersection);
    }
    
    return Size3D(xIntersection);
}

Size3D Rectangle::getIntersection(Circle& circle)
{
    float halfWidth  = _size.w / 2;
    float halfHeight = _size.h / 2;

    float circleDistanceX = fabs(circle.getLocation().x - (_location.x /*+ halfWidth*/));
    float circleDistanceY = fabs(circle.getLocation().y - (_location.y /*+ halfHeight*/));

    //if (circleDistanceY > (halfHeight + circle.getRadius() || circleDistanceX > (halfWidth + circle.getRadius())
            //return Size2D();

    /*if (circleDistanceX <= halfWidth)
            return true;

    if (circleDistanceY <= halfHeight)
            return true;*/

    float cornerDistance = sqrt(pow(circleDistanceX - halfWidth, 2) + pow(circleDistanceY - halfHeight, 2));
    float intersection   = circle.getRadius() - cornerDistance;

    //return (cornerDistance <= circle.radius);
    return Size3D(intersection, intersection);

    /*ORIGINAL CODE

     private function intersects(circle:Circle, rect:Rectangle):Boolean
     {
             var halfWidth:Number = rect.width / 2;
             var halfHeight:Number = rect.height / 2;

             var circleDistanceX:Number = Math.abs(circle.x - (rect.x + halfWidth));
             var circleDistanceY:Number = Math.abs(circle.y - (rect.y + halfHeight));

             if (circleDistanceX > (halfWidth + circle.radius))
             return false;

             if (circleDistanceY > (halfHeight + circle.radius))
             return false;

             if (circleDistanceX <= halfWidth)
             return true;

             if (circleDistanceY <= halfHeight)
             return true;

             var cornerDistance:Number = Math.sqrt(Math.pow(circleDistanceX - halfWidth, 2) + Math.pow(circleDistanceY - halfHeight, 2));

             return (cornerDistance <= circle.radius);
     }
     */
}

Circle Rectangle::getBoundingCircle()
{
	//float radius = fminf(_size.getW(),_size.getH()) / 2;
	//return Circle(_location,radius);
    float a = _size.w;
    float b = _size.h;
    float c = sqrt(a*a + b*b);
    return Circle(_location,c/2);
}


bool Rectangle::contains(Point3D& point)
{
    /*float middleWidthSize  = _size.w() / 2;
    float middleHeightSize = _size.h() / 2;
    return _location.x() - middleWidthSize <= point.x()  && _location.x() + middleWidthSize >= point.x()
        && _location.y() - middleHeightSize <= point.y() && _location.y() + middleHeightSize >= point.y();*/
    
    float middleWidthSize  = _size.w / 2;
    float middleHeightSize = _size.h / 2;
    float minX             = _location.x - middleWidthSize;
    float minY             = _location.y - middleHeightSize;
    float maxX             = _location.x + middleWidthSize;
    float maxY             = _location.y + middleHeightSize;
    
    return point.x > minX && point.x < maxX &&
           point.y > minY && point.y < maxY;
}

float Rectangle::getMaxX()
{
    return _location.x + (_size.w / 2);
}

float Rectangle::getMinX()
{
    return _location.x - (_size.w / 2);
}

float Rectangle::getMaxY()
{
    return _location.y + (_size.h / 2);
}

float Rectangle::getMinY()
{
    return _location.y - (_size.h / 2);
}
