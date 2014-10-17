//
//  GLSprite.cpp
//  PVRTextureLoader
//
//  Created by Leonardo on 9/23/13.
//
//

#include "GLSprite.h"
#include "GLDriver.h"

GLSprite::GLSprite(GLTexture* texture, Rectangle textureRectangle)
{
    initializeVariables();
    _texture          = texture;
    _textureRectangle = textureRectangle;
    generateVertices();
    generateTextureCoordinates();
}

void GLSprite::initializeVariables()
{
    _z                    = 0;
    _pointsNumber         = 4;
    _coordinatesNumber    = 2;
    _vertices             = NULL;
    _texture              = NULL;
    _textureCoordinates   = NULL;
    _transformationsState = new GLTransformationsState();
}

GLSprite::~GLSprite()
{
    releaseTextureCoordinates();
    
    delete _transformationsState;
    _transformationsState = NULL;
}

float GLSprite::z()           { return _z; }
void GLSprite::z(float value) { _z = value; }

Point3D GLSprite::screenPositionToWorldLocalPosition(Point3D& screenPosition)
{
    return GLDriver::screenPositionToWorldPosition(screenPosition, _transformationsState);
}

bool GLSprite::hitTest(Point3D& screenPosition)
{
    Point3D localPosition     = screenPositionToWorldLocalPosition(screenPosition);
    Rectangle spriteRectangle = Rectangle(Point3D(), _textureRectangle.size());
    
    return spriteRectangle.contains(localPosition);
}

bool GLSprite::prepareRender()
{
    if(_texture != NULL && _transformations.isVisible() && _transformations.color().a() > 0)
    {
        GLDriver::pushMatrix();
        GLDriver::applyTransformations(_transformations);
        GLDriver::getTransformationsState(_transformationsState);
        
        return true;
    }
    
    return false;
}


void GLSprite::render()
{
    if(prepareRender())
    {
        GLDriver::renderTexture2DTriangleStrip(_texture->textureData()->identifier, _vertices, _textureCoordinates, _pointsNumber, _coordinatesNumber);
        GLDriver::popMatrix();
        GLDriver::color4f(FULL_COLOR, FULL_COLOR, FULL_COLOR, FULL_COLOR);
    }
    
    GLRenderable::render();
}

void GLSprite::generateVertices()
{
    releaseVertices();
    
    _vertices = new float[_coordinatesNumber * _pointsNumber];
    
    float midWidth  = _textureRectangle.size().w / 2;
    float midHeight = _textureRectangle.size().h / 2;
    float minX      = - midWidth;
    float minY      = - midHeight;
    float maxX      = midWidth;
    float maxY      = midHeight;
	
    _vertices[0] = minX;
    _vertices[1] = maxY;
    
    _vertices[2] = maxX;
    _vertices[3] = maxY;
    
    _vertices[4] = minX;
    _vertices[5] = minY;
    
    _vertices[6] = maxX;
    _vertices[7] = minY;
}

void GLSprite::releaseVertices()
{
    if(_vertices)
    {
        delete _vertices;
        _vertices = NULL;
    }
}

void GLSprite::generateTextureCoordinates()
{
    if(_texture)
    {
        releaseTextureCoordinates();
        
        _textureCoordinates = new float[_coordinatesNumber * _pointsNumber];
        
        float textureW           = _texture->textureData()->size.w;
        float textureH           = _texture->textureData()->size.h;
        bool isYInvertedInMemory = _texture->textureData()->isYInvertedInMemory;
        
        float coordinatesX         = _textureRectangle.location().x + textureW / 2;
        float coordinatesY         = -(_textureRectangle.location().y + textureH / 2);
        float coordinatesMidWidth  = _textureRectangle.size().w / 2;
        float coordinatesMidHeight = _textureRectangle.size().h / 2;
        
        float minX = (coordinatesX - coordinatesMidWidth) / textureW;
        float minY = isYInvertedInMemory ? (coordinatesY - coordinatesMidHeight) / textureH : (coordinatesY + coordinatesMidHeight) / textureH;
        float maxX = (coordinatesX + coordinatesMidWidth) / textureW;
        float maxY = isYInvertedInMemory ? (coordinatesY + coordinatesMidHeight) / textureH : (coordinatesY - coordinatesMidHeight) / textureH;
        
        _textureCoordinates[0] = minX;
        _textureCoordinates[1] = maxY;
        
        _textureCoordinates[2] = maxX;
        _textureCoordinates[3] = maxY;
        
        _textureCoordinates[4] = minX;
        _textureCoordinates[5] = minY;
        
        _textureCoordinates[6] = maxX;
        _textureCoordinates[7] = minY;
    }
}

void GLSprite::releaseTextureCoordinates()
{
    if(_textureCoordinates)
    {
        delete _textureCoordinates;
        _textureCoordinates = NULL;
    }
}