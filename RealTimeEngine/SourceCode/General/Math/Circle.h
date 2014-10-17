/*
 *  Circle.h
 *  NOTLD
 *
 *  Created by Leonardo Rojas on 12/4/09.
 *  Copyright 2009 FPL. All rights reserved.
 *
 */

#ifndef CIRCLE_H
#define CIRCLE_H

#include "Size3D.h"
#include "Point3D.h"

class Circle
{
public:
    explicit Circle(float radius = 0);
    explicit Circle(Point3D location, float radius = 0);

    Size3D getIntersection(Circle circle);
    Point3D getLocation();
    float getRadius();
    void setRadius(float radius);
    float getDistance(Point3D point);
    virtual bool contains(Point3D point);
	
protected:
    float    _radius;
    Point3D _location;
};

#endif
