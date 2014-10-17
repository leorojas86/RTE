//
//  ScreenInput.h
//  PVRTextureLoader
//
//  Created by Leonardo on 10/26/13.
//
//

#ifndef SCREEN_INPUT_H
#define SCREEN_INPUT_H

#include "Point3D.h"
#include "Size3D.h"
#include <vector>

class ScreenInput
{
public:
    static std::vector<Point3D> touches;
    static Size3D screenSize;
};

#endif
