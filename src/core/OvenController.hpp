//
//  OvenController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "TopDownEntityPhysicsController.hpp"

class OvenPhysicsController : public TopDownEntityPhysicsController
{
    DECL_RTTI;
    DECL_EntityController_create(EntityPhysicsController);
    
public:
    OvenPhysicsController(Entity* e) : TopDownEntityPhysicsController(e) {}
    virtual ~OvenPhysicsController() {}
    
protected:
    virtual void onCollision(Entity* e) override;
};
