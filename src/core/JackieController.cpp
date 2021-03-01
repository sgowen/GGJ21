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
    
    uint8_t playerID = _entity->entityDef()._data.getUInt("playerID");
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

IMPL_RTTI(JackiePhysicsController, TopDownPhysicsController)
IMPL_EntityController_create(JackiePhysicsController, EntityPhysicsController)

void JackiePhysicsController::onCollision(Entity* e)
{
    if (e->controller()->getRTTI().isDerivedFrom(CrystalController::rtti))
    {
        e->physicsController<TopDownPhysicsController>()->push(_entity->controller<JackieController>()->_stats._dir, CFG_MAIN.jackiePushForce());
    }
}

IMPL_RTTI(JackieRenderController, EntityRenderController)
IMPL_EntityController_create(JackieRenderController, EntityRenderController)

std::string JackieRenderController::getTextureMapping()
{
    JackieController* ec = _entity->controller<JackieController>();
    
    switch (ec->_stats._dir)
    {
        case EDIR_UP:
            return "JACKIE_UP";
        case EDIR_LEFT:
        case EDIR_RIGHT:
            return "JACKIE_LEFT";
        case EDIR_DOWN:
            return "JACKIE_DOWN";
    }
    
    return EntityRenderController::getTextureMapping();
}
