//
//  CrystalController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "EntityController.hpp"

class CrystalController : public EntityController
{
    friend class CrystalNetworkController;
    
    DECL_RTTI;
    DECL_EntityController_create;
    
public:
    CrystalController(Entity* e);
    virtual ~CrystalController() {}
    
    virtual void update();
    virtual void onMessage(uint16_t message, void* data = NULL);
    
    float getWidthForRender();
    void push(int dir);
    
private:
    enum State
    {
        STAT_IDLE      = 0,
        STAT_EXPLODING = 1
    };
};
