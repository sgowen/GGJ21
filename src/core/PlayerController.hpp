//
//  PlayerController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <GowEngine/GowEngine.hpp>

#include <string>

class PlayerController : public EntityController
{    
    DECL_RTTI;
    DECL_EntityController_create(PlayerController);
    
public:
    PlayerController(Entity* e) : EntityController(e) {}
    virtual ~PlayerController() {}
    
    virtual void processInput(InputState* is, bool isLive);
    
protected:
    enum State
    {
        STAT_IDLE = 0,
        STAT_MOVING = 1
    };
};
