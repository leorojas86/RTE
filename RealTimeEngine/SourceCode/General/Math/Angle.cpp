/*
 *  Angle.cpp
 *  NOTLD
 *
 *  Created by Leonardo Rojas on 12/14/09.
 *  Copyright 2009 FPL. All rights reserved.
 *
 */

#include "Angle.h"
#include <math.h>

Angle::Angle(float degrees)
{
    this->degrees(degrees);
}

Angle::Angle(double radians)
{
    this->radians(radians);
}

float Angle::degrees()
{
    return _degrees;
}

void Angle::degrees(float value)
{
    _degrees = contraintDegreesTo360(value);
    _radians = degreesToRadians(_degrees);
}

double Angle::radians()
{
    return _radians;
}

void Angle::radians(double value)
{
    _degrees = radiansToDegrees(value);
    _degrees = contraintDegreesTo360(_degrees);
    _radians = degreesToRadians(_degrees);
}

void Angle::add(Angle angle)
{
    degrees(_degrees + angle.degrees());
}

void Angle::remove(Angle angle)
{
    degrees(_degrees - angle.degrees());
}

bool Angle::isEmpty()
{
    return _degrees == MIN_DEGREES;
}

float Angle::toScale()
{
    return cosf(_radians);
}

float Angle::radiansToDegrees(double radians)
{
    return RADIANS_TO_DEGREES * radians;
}

double Angle::degreesToRadians(float degrees)
{
    return DEGREES_TO_RADIANS * degrees;
}

float Angle::getDifference(Angle angle)
{
    float originAngle  = contraintDegreesTo180(_degrees);
    float destinyAngle = contraintDegreesTo180(angle.degrees());
    float difference   = destinyAngle - originAngle;
    float obsoluteDifference = fabs(difference);
    
    if(obsoluteDifference > MIDDLE_DEGREES)// correcting calculation when the absolute difference > 180
    {
        if(difference > MIN_DEGREES)
           difference = -(MAX_DEGREES - obsoluteDifference);
        else
           difference =   MAX_DEGREES - obsoluteDifference;
    }
    
    return difference;
}

//Transform the degrees in a value between 0 and 360 
float Angle::contraintDegreesTo360(float degrees)
{
    /*if(degrees < MIN_DEGREES)
       return degrees + MAX_DEGREES; //removes the negative degrees
    else if(degrees > MAX_DEGREES)
       return degrees - MAX_DEGREES; // removes the degrees > 360*/
    return degrees;
}

//Transform the degrees in a value between -180 and 180 
float Angle::contraintDegreesTo180(float degrees)
{
	if(degrees > MIDDLE_DEGREES)
		return -(MAX_DEGREES - degrees);//Make a angle > 180 an angle negative from 0 to -180
    
	return degrees;
}

void Angle::invert()
{
    degrees(MAX_DEGREES - _degrees);
}
