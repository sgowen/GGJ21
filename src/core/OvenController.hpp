//
//  OvenController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <GowEngine/GowEngine.hpp>

class OvenController : public EntityController
{
    DECL_RTTI;
    DECL_EntityController_create(EntityController);
    
public:
    OvenController(Entity* e) : EntityController(e) {}
    virtual ~OvenController() {}
    
    virtual void onCollision(Entity* e) override;
};
