//
//  JackieController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

IMPL_RTTI(JackieController, PlayerController)
IMPL_EntityController_create(JackieController, EntityController)

void JackieController::processInput(InputState* is, bool isLive)
{
    PlayerController::processInput(is, isLive);
    
    uint8_t playerID = _entity->metadata().getUInt("playerID");
    InputState::PlayerInputState* pis = is->playerInputStateForID(playerID);
    if (pis == NULL)
    {
        return;
    }
    
    uint8_t piss = pis->_inputState;
    if (IS_BIT_SET(piss, GISF_CONFIRM))
    {
        // TODO
    }
    else if (IS_BIT_SET(piss, GISF_CANCEL))
    {
        // TODO
    }
}

void JackieController::onCollision(Entity* e)
{
    if (e->controller()->getRTTI().isDerivedFrom(CrystalController::rtti))
    {
        e->physicsController<TopDownPhysicsController>()->push(_entity->state()._stateFlags, CFG_MAIN.jackiePushForce());
    }
}
