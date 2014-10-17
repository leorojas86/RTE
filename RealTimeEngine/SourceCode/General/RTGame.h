//
//  RTGame.h
//
//  Created by Leonardo on 10/26/13.
//
//

#ifndef RT_GAME_H
#define RT_GAME_H

#include "GLRenderable.h"
#include <string>
#include "Size3D.h"

class RTGame
{
public:
    RTGame(std::string resourcesRootPath);
    ~RTGame();
    
    GLRenderable* rootRenderable();
    
    virtual void render(Size3D screenSize);
    virtual void execute();
    
protected:
    GLRenderable* _rootRenderable;
};

#endif