//
//  JackiePlayerController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "JackiePlayerController.hpp"

#include "Entity.hpp"
#include "CrystalController.hpp"

IMPL_RTTI(JackiePlayerController, PlayerController)
IMPL_EntityController_create(JackiePlayerController)

std::string JackiePlayerController::getTextureMapping()
{
    switch (_stats._dir)
    {
        case PDIR_UP:
            return "JACKIE_UP";
        case PDIR_LEFT:
        case PDIR_RIGHT:
            return "JACKIE_LEFT";
        case PDIR_DOWN:
            return "JACKIE_DOWN";
        default:
            assert(false);
    }
}

void JackiePlayerController::onCollision(Entity* e)
{
    if (e->controller()->getRTTI().isDerivedFrom(CrystalController::rtti))
    {
        CrystalController* ec = static_cast<CrystalController*>(e->controller());
        
        ec->push(_stats._dir);
    }
}
