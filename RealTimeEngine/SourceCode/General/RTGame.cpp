//
//  RTGame.cpp
//
//  Created by Leonardo on 10/26/13.
//
//

#include "RTGame.h"
#include "Resources.h"
#include "ScreenInput.h"

RTGame::RTGame(std::string resourcesRootPath)
{
    Resources::rootPath = resourcesRootPath;
    _rootRenderable     = new GLRenderable();
}

RTGame::~RTGame()
{
    delete _rootRenderable;
    _rootRenderable = NULL;
}

GLRenderable* RTGame::rootRenderable() { return _rootRenderable; }

void RTGame::render(Size3D screenSize)
{
    ScreenInput::screenSize = screenSize;
    _rootRenderable->render();
}

void RTGame::execute()
{
    //DO WHATEVER YOU NEED HERE!
}