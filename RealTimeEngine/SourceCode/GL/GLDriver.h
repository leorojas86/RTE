//
//  IGLDriver.h
//  PVRTextureLoader
//
//  Created by Leonardo on 9/22/13.
//
//

#ifndef GL_DRIVER_H
#define GL_DRIVER_H

#include "Transformations.h"
#include <string>

#define NONE_TEXTURE 0

struct GLTextureData
{
    unsigned int identifier;
    Size3D size;
    int type;
    int colorFormat;
    int filter;
    int filterParameter;
    bool isYInvertedInMemory;
    
    GLTextureData()
    {
        identifier          = NONE_TEXTURE;
        type                = 0;
        colorFormat         = 0;
        filter              = 0;
        filterParameter     = 0;
        isYInvertedInMemory = false;
    }
    
    GLTextureData(unsigned int textureId, Size3D imageSize, int textureType, int textureColorFormat, int textureFilter, int textureFilterParameter, bool isYInverted)
    {
        identifier          = textureId;
        size                = imageSize;
        type                = textureType;
        colorFormat         = textureColorFormat;
        filter              = textureFilter;
        filterParameter     = textureFilterParameter;
        isYInvertedInMemory = isYInverted;
    }
};

struct GLTransformationsState
{
    float* modelViewMatrix;
    float* projMatrix;
    int* viewport;
    
    GLTransformationsState()
    {
        modelViewMatrix = new float[16];
        projMatrix      = new float[16];
        viewport        = new int[4];
    }
    
    ~GLTransformationsState()
    {
        delete modelViewMatrix;
        delete projMatrix;
        delete viewport;
    }
};

class GLDriver
{    
public:

    //TEXTURES
    static GLTextureData* loadTextureData(std::string path, int type, int filter, int filterParameter);
    static void unloadTexture(GLTextureData* textureData);
    
    //TRANSFORMATIONS
    static void pushMatrix();
    static void applyTransformations(Transformations& transformations);
    static void color4f(float r, float g, float b, float a);
    static void getTransformationsState(GLTransformationsState* transformationsState);
    static Point3D screenPositionToWorldPosition(Point3D& position, GLTransformationsState* transformationsState);
    static void popMatrix();
    
    //CAMERA
    static void clearBufferColor(Color& color);
    static void setUpOrthographicProjection(Size3D& screenSize);
    
    //RENDERING
    static void renderTexture2DTriangleStrip(unsigned int textureId, float* vertices, float* textureCoordinates, int pointsNumber, int coordinatesNumber);
};

#endif