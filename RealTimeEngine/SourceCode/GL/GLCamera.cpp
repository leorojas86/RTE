//
//  GLCamera.cpp
//  PVRTextureLoader
//
//  Created by Leonardo on 9/28/13.
//
//

#include "GLCamera.h"
#include "GLDriver.h"
#include "ScreenInput.h"

GLCamera::GLCamera(Color clearColor)
{
    _clearColor = clearColor;
}

void GLCamera::render()
{
    GLDriver::clearBufferColor(_clearColor);
    GLDriver::setUpOrthographicProjection(ScreenInput::screenSize);
    GLRenderable::render();
}