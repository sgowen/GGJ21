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
    OvenController(Entity* e) : EntityController(e) {}
    virtual ~OvenController() {}
    
    virtual void onCollision(Entity* e);
};
