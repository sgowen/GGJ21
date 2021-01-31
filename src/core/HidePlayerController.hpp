//
//  HidePlayerController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "PlayerController.hpp"

class HidePlayerController : public PlayerController
{
    friend class HidePlayerNetworkController;
    
    DECL_RTTI;
    DECL_EntityController_create;
    
public:
    HidePlayerController(Entity* e);
    virtual ~HidePlayerController() {}
};

class HidePlayerNetworkController : public PlayerNetworkController
{
    DECL_EntityNetworkController_create;
    
public:
    HidePlayerNetworkController(Entity* e, bool isServer);
    virtual ~HidePlayerNetworkController() {}
    
private:
    HidePlayerController* _controller;
};
