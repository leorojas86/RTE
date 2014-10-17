#include "Point3D.h"
#include "math.h"

Point3D::Point3D(float xPos, float yPos, float zPos)
{
    x = xPos;
    y = yPos;
    z = zPos;
}

Angle Point3D::getAngle(Point3D point)
{
   float a = point.y - y;
   float b = point.x - x;
   return Angle(atan2(a, b));
}

float Point3D::getDistance(Point3D point)
{
     float xDistance = getDistance(x, point.x);
     float yDistance = getDistance(y, point.y);

     //C2=A2+B2;
     double distance = sqrt((xDistance * xDistance) + (yDistance * yDistance));
     return (float)distance;
}

Point3D Point3D::getRotatedPoint(Point3D point,Angle angle)
{
    Angle currentAngle = getAngle(point);
    angle.add(currentAngle);
    float radius = getDistance(point);
    return getPointAtDistance(radius, angle);
}

float Point3D::getDistance(float x, float x2)
{
    return fmaxf(x, x2) - fminf(x, x2);
}

Point3D Point3D::getPointAtDistance(float distance, Angle angle)
{
    float xOffset = cos(angle.radians())  * distance;
    float yOffset = sin(angle.radians())  * distance;
    
    return Point3D(x + xOffset, y + yOffset);
}

bool Point3D::isEmpty()
{
    return x == 0 && y == 0;
}

Point3D Point3D::add(Point3D point)
{
    return Point3D(x + point.x, y + point.y);
}

Point3D Point3D::subtract(Point3D point)
{
    return Point3D(x - point.x, y - point.y);
}