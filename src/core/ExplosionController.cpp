//
//  ExplosionController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/v/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

IMPL_RTTI(ExplosionController, EntityController)
IMPL_EntityController_create(ExplosionController, EntityController)

void ExplosionController::update()
{
    if (_entity->state()._stateTime >= 42)
    {
        _entity->requestDeletion();
    }
}
