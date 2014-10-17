/*
 *  Size2D.h
 *  MyTestIPhoneApp
 *
 *  Created by Leonardo Rojas on 12/2/09.
 *  Copyright 2009 FPL. All rights reserved.
 *
 */

#ifndef SIZE3D_H
#define SIZE3D_H

class Size3D
{
public: 
    explicit Size3D(float width = 0, float height = 0, float depth = 0);

    float w;
    float h;
    float d;
    
    float aspecRatio();

    void multiply(Size3D& size);

    virtual bool isEmpty();

    static bool equals(Size3D& size, Size3D& other);
};

#endif
