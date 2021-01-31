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
    friend class JackiePlayerNetworkController;
    
    DECL_RTTI;
    DECL_EntityController_create;
    
public:
    JackiePlayerController(Entity* e);
    virtual ~JackiePlayerController() {}
    
    virtual std::string getTextureMapping(uint8_t state);
    virtual void onCollision(Entity* e);
};

class JackiePlayerNetworkController : public PlayerNetworkController
{
    DECL_EntityNetworkController_create;
    
public:
    JackiePlayerNetworkController(Entity* e, bool isServer);
    virtual ~JackiePlayerNetworkController() {}
    
private:
    JackiePlayerController* _controller;
};
