//
//  ExampleGame.h
//
//  Created by Leonardo on 10/26/13.
//
//

#ifndef EXAMPLE_GAME_H
#define EXAMPLE_GAME_H

#include "RTGame.h"

class ExampleGame: public RTGame
{
public:
    ExampleGame(std::string resourcesRootPath);
    ~ExampleGame();

    virtual void execute();
};

#endif
