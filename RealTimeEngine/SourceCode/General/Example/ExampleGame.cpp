//
//  ExampleGame.cpp
//  PVRTextureLoader
//
//  Created by Leonardo on 10/26/13.
//
//

#include "ExampleGame.h"
#include "GLTexture.h"
#include "GLSprite.h"
#include "GLCamera.h"
#include "ScreenInput.h"
#include "LogUtils.h"
#include "StringUtils.h"

GLTexture* glTexture;
GLSprite* glSprite;
GLCamera* glCamera;

ExampleGame::ExampleGame(std::string resourcesRootPath):RTGame(resourcesRootPath)
{
    //glTexture    = new GLTexture("ExampleResources/ArialMT_30.png");
    glTexture  = new GLTexture("ExampleResources/Screenshot.jpg");
    //glTexture  = new GLTexture("ExampleResources/Screenshot.png");
    //glTexture  = new GLTexture("ExampleResources/textures.pvr");
    glSprite     = new GLSprite(glTexture, Rectangle(Point3D(), Size3D(glTexture->textureData()->size.w, glTexture->textureData()->size.h)));
    //glSprite   = new GLSprite(glTexture, Rectangle(Point3D(0, -glTexture->textureData()->size.h/4.0f), Size3D(glTexture->textureData()->size.w, glTexture->textureData()->size.h/2.0f)));
    //glSprite->z(5);
    //glSprite->transformations().translation(Point3D(-90, -100));
    //glSprite->transformations2D().translation(Point2D(-0.3, 0));
    //glSprite->transformations2D().rotation(Angle(75.0f));
    glSprite->transformations().scale(Size3D(0.5f, 0.5f, 1));
    
    glCamera = new GLCamera(Color(1,0.5,0.5,1));
    glCamera->children().push_back(glSprite);
    
    _rootRenderable->children().push_back(glCamera);
}

void ExampleGame::execute()
{
    if(ScreenInput::touches.size() > 0)
    {
        Point3D firstTouch = ScreenInput::touches[0];
        
        if(glSprite->hitTest(firstTouch))
            glSprite->transformations().color(Color(0.5, 1, 0.5, 0.5));
        else
            glSprite->transformations().color(Color(1, 1, 1, 1));
    }
}