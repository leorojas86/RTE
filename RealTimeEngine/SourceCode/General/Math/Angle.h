/*
 *  Angle.h
 *  NOTLD
 *
 *  Created by Leonardo Rojas on 12/14/09.
 *  Copyright 2009 FPL. All rights reserved.
 *
 */

#ifndef ANGLE_H
#define ANGLE_H

#define MAX_DEGREES        360.0f
#define MIN_DEGREES        0.0f
#define MIDDLE_DEGREES     180.0f
#define RECT_ANGLE         90.0f
#define RADIANS_TO_DEGREES MIDDLE_DEGREES / M_PI
#define DEGREES_TO_RADIANS M_PI / MIDDLE_DEGREES

class Angle
{
public:
    explicit Angle(float degrees = MIN_DEGREES);
    explicit Angle(double radians);

    float degrees();
    void degrees(float value);

    double radians();
    void radians(double value);

    void add(Angle angle);
    void remove(Angle angle);
    float getDifference(Angle angle);

    bool isEmpty();
    float toScale();

    static float radiansToDegrees(double radians);
    static double degreesToRadians(float degrees);
    //Transform the degrees in a value between 0 and 360
    static float contraintDegreesTo360(float degrees);
    //Transform the degrees in a value between -180 and 180
    float contraintDegreesTo180(float degrees);

    void invert();
private:
    double _radians;
    float _degrees;
};

#endif
