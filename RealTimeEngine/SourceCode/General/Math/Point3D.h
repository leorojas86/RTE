#ifndef POINT3D_H
#define POINT3D_H

#include "Angle.h"

class Point3D
{
public:
    explicit Point3D(float xPos = 0, float yPos = 0, float zPos = 0);

    float x;
    float y;
    float z;
	
    Angle getAngle(Point3D point);
    float  getDistance(Point3D point);
    static float getDistance(float x, float x2);
    Point3D getRotatedPoint(Point3D point,Angle angle);
    Point3D getPointAtDistance(float distance, Angle angle);
    Point3D getRotatedPoint(float radius,double radians);

    virtual bool isEmpty();

    Point3D add(Point3D point);
    Point3D subtract(Point3D point);
};

#endif
