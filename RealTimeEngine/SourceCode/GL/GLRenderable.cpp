//
//  GLRenderable.cpp
//  PVRTextureLoader
//
//  Created by Leonardo on 9/28/13.
//
//

#include "GLRenderable.h"

GLRenderable::GLRenderable()
{
}

std::vector<GLRenderable*>& GLRenderable::children() { return _children; }

Transformations& GLRenderable::transformations()         { return _transformations; }
void GLRenderable::transformations(Transformations value){ _transformations = value; }

void GLRenderable::render()
{
    for(std::vector<GLRenderable*>::iterator it = _children.begin(); it != _children.end(); it++)
        (*it)->render();
}