//
//  OvenController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

IMPL_RTTI(OvenController, EntityController)
IMPL_EntityController_create(OvenController)

void OvenController::onCollision(Entity* e)
{
    if (e->controller()->getRTTI().isDerivedFrom(CrystalController::rtti))
    {
        e->message(MSG_ENCOUNTER);
    }
}
