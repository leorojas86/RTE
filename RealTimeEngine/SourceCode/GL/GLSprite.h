//
//  GLSprite.h
//  PVRTextureLoader
//
//  Created by Leonardo on 9/23/13.
//
//

#ifndef GL_SPRITE_H
#define GL_SPRITE_H

#include "GLTexture.h"
#include "Rectangle.h"
#include "GLRenderable.h"

class GLSprite: public GLRenderable
{
public:
	
    GLSprite(GLTexture* texture, Rectangle textureRectangle);
    ~GLSprite();
    
    float z();
    void z(float value);
    
    Point3D screenPositionToWorldLocalPosition(Point3D& screenPosition);
    
    bool hitTest(Point3D& screenPosition);

    virtual void render();
	
	
private:
    
    void initializeVariables();
    
    void generateVertices();
    void releaseVertices();
	bool prepareRender();
    
    void generateTextureCoordinates();
    void releaseTextureCoordinates();
	
    float _z;
    float* _vertices;
    int _coordinatesNumber;
    int _pointsNumber;
    
    GLTexture* _texture;
    float*     _textureCoordinates;
    Rectangle  _textureRectangle;
    
    GLTransformationsState* _transformationsState;
};

#endif /* defined(__PVRTextureLoader__GLSprite__) */
