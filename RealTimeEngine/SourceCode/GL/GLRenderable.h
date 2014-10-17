//
//  GLRenderable.h
//  PVRTextureLoader
//
//  Created by Leonardo on 9/28/13.
//
//

#ifndef GL_RENDERABLE_H
#define GL_RENDERABLE_H

#include "Transformations.h"
#include <vector>

class GLRenderable
{
public:
    GLRenderable();
    
    std::vector<GLRenderable*>& children();
    
    Transformations& transformations();
    void transformations(Transformations value);
    
    virtual void render();
    
protected:
    std::vector<GLRenderable*> _children;
    
    Transformations _transformations;
};

#endif /* defined(__PVRTextureLoader__GLRenderable__) */
