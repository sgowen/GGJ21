//
//  OvenController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "EntityController.hpp"

class OvenController : public EntityController
{
    friend class OvenNetworkController;
    
    DECL_RTTI;
    DECL_EntityController_create;
    
public:
    OvenController(Entity* e);
    virtual ~OvenController() {}
    
    virtual void onCollision(Entity* e);
};

#include "EntityNetworkController.hpp"

class OvenNetworkController : public EntityNetworkController
{
    DECL_EntityNetworkController_create;
    
public:
    OvenNetworkController(Entity* e, bool isServer);
    virtual ~OvenNetworkController() {}
    
    virtual void read(InputMemoryBitStream& ip);
    virtual uint16_t write(OutputMemoryBitStream& op, uint16_t dirtyState);
    
    virtual void recallNetworkCache();
    virtual uint16_t getDirtyState();
    
private:
    OvenController* _controller;
};
