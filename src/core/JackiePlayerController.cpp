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

IMPL_RTTI(JackiePlayerController, PlayerController);
IMPL_EntityController_create(JackiePlayerController);

JackiePlayerController::JackiePlayerController(Entity* e) : PlayerController(e)
{
    // Empty
}

std::string JackiePlayerController::getTextureMapping(uint8_t state)
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
    if (!_entity->getNetworkController()->isServer())
    {
        return;
    }
    
    if (e->getController()->getRTTI().derivesFrom(CrystalController::rtti))
    {
        CrystalController* ec = static_cast<CrystalController*>(e->getController());
        
        ec->push(_stats._dir);
    }
}

IMPL_EntityNetworkController_create(JackiePlayerNetworkController);

JackiePlayerNetworkController::JackiePlayerNetworkController(Entity* e, bool isServer) : PlayerNetworkController(e, isServer)
{
    // Empty
}
