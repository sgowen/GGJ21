//
//  OvenController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "OvenController.hpp"

#include "Entity.hpp"
#include "CrystalController.hpp"
#include "MainConfig.hpp"

IMPL_RTTI(OvenPhysicsController, TopDownEntityPhysicsController)
IMPL_EntityController_create(OvenPhysicsController, EntityPhysicsController)

void OvenPhysicsController::onCollision(Entity* e)
{
    if (e->controller()->getRTTI().isDerivedFrom(CrystalController::rtti))
    {
        e->message(MSG_ENCOUNTER);
    }
}
