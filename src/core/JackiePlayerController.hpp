//
//  JackiePlayerController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "PlayerController.hpp"

class JackiePlayerController : public PlayerController
{    
    DECL_RTTI;
    DECL_EntityController_create;
    
public:
    JackiePlayerController(Entity* e) : PlayerController(e) {}
    virtual ~JackiePlayerController() {}
    
    virtual std::string getTextureMapping();
    virtual void onCollision(Entity* e);
    virtual void processInput(InputState* inputState, bool isLive);
};
