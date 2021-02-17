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
    DECL_RTTI;
    DECL_EntityController_create(EntityController);
    
public:
    CrystalController(Entity* e) : EntityController(e) {}
    virtual ~CrystalController() {}
    
    virtual void onMessage(uint16_t message, void* data = NULL) override;
    
    void push(int dir);
};
