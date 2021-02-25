//
//  JackieController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "JackieController.hpp"

#include "Entity.hpp"
#include "CrystalController.hpp"
#include "GameInputState.hpp"
#include "Macros.hpp"
#include "AudioEngineFactory.hpp"
#include "StringUtil.hpp"
#include "EntityRenderController.hpp"
#include "MainConfig.hpp"

IMPL_RTTI(JackieController, PlayerController)
IMPL_EntityController_create(JackieController, EntityController)

void JackieController::processInput(InputState* inputState, bool isLive)
{
    PlayerController::processInput(inputState, isLive);
    
    GameInputState* is = static_cast<GameInputState*>(inputState);
    GameInputState::PlayerInputState* pis = is->getPlayerInputStateForID(getPlayerID());
    if (pis == NULL)
    {
        return;
    }
    
    uint8_t piss = pis->inputState();
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
        e->physicsController<TopDownPhysicsController>()->push(_entity->controller<JackieController>()->_stats._dir, CFG_MAIN._jackiePushForce);
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
