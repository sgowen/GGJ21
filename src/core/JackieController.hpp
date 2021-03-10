//
//  JackieController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <GowEngine/GowEngine.hpp>

#include "PlayerController.hpp"

class JackieController : public PlayerController
{    
    DECL_RTTI;
    DECL_EntityController_create(EntityController);
    
public:
    JackieController(Entity* e) : PlayerController(e) {}
    virtual ~JackieController() {}
    
    virtual void processInput(InputState* is, bool isLive) override;
    virtual void onCollision(Entity* e) override;
};
