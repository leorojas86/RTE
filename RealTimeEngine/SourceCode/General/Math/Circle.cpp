/*
 *  Circle.cpp
 *  NOTLD
 *
 *  Created by Leonardo Rojas on 12/4/09.
 *  Copyright 2009 FPL. All rights reserved.
 *
 */

#include "Circle.h"
#include "Angle.h"
#include <math.h>
#include <stdio.h>

Circle::Circle(float radius)
{
	_radius = radius;
}

Circle::Circle(Point3D location, float radius)
{
	_radius   = radius;
	_location = location;
}
		
Size3D Circle::getIntersection(Circle circle)
{
	float circlesDistance = _location.getDistance(circle.getLocation());
	float intersection    = _radius +  circle.getRadius() - circlesDistance;
	return Size3D(intersection,intersection);
	/*Point2D2 circleLocation = circle.getLocation();
	float xDistance = fmaxf(_location.getX(),circleLocation.getX()) - fminf(_location.getX(),circleLocation.getX());
	float xIntersection = _radius + circle.getRadius() - xDistance;
	if(xIntersection > 0)
	{
		float yDistance = fmaxf(_location.getY(),circleLocation.getY()) - fminf(_location.getY(),circleLocation.getY());
		float yIntersection = _radius + circle.getRadius() - yDistance;
		return Size2D(xIntersection,yIntersection);
	}
	return Size2D(xIntersection);*/
}

Point3D Circle::getLocation()
{
	return _location;
}

float Circle::getRadius()
{
	return _radius;
}

void Circle::setRadius(float radius)
{
	_radius = radius;
}

float Circle::getDistance(Point3D point)
{
	return _location.getDistance(point) - _radius;
}

bool Circle::contains(Point3D point)
{
	return _location.getDistance(point) <= _radius;
}