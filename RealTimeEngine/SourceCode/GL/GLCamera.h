//
//  GLCamera.h
//  PVRTextureLoader
//
//  Created by Leonardo on 9/28/13.
//
//

#ifndef GL_CAMERA_H
#define GL_CAMERA_H

#include "Size3D.h"
#include "Color.h"
#include "GLRenderable.h"

class GLCamera: public GLRenderable
{
public:
    GLCamera(Color clearColor);
    
    virtual void render();
    
private:
    Color _clearColor;
};

#endif
