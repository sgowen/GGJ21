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
#include "GowAudioEngine.hpp"
#include "StringUtil.hpp"
#include "EntityRenderController.hpp"

IMPL_RTTI(JackieController, PlayerController)
IMPL_EntityController_create(JackieController, EntityController)

bool ugh = false;
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
    if (IS_BIT_SET(piss, GISF_CONFIRM) && !ugh)
    {
        if (isLive)
        {
            ugh = true;
            LOG("play sound 1");
            GOW_AUDIO.playSound(_entity->renderController()->getSoundMapping(1));
        }
    }
    else if (IS_BIT_SET(piss, GISF_CANCEL) && ugh)
    {
        if (isLive)
        {
            ugh = false;
            LOG("play sound 2");
            GOW_AUDIO.playSound(_entity->renderController()->getSoundMapping(1));
        }
    }
}

IMPL_RTTI(JackiePhysicsController, TopDownPhysicsController)
IMPL_EntityController_create(JackiePhysicsController, EntityPhysicsController)

void JackiePhysicsController::onCollision(Entity* e)
{
    if (e->controller()->getRTTI().isDerivedFrom(CrystalController::rtti))
    {
        CrystalController* ec = e->controller<CrystalController>();
        ec->push(_entity->controller<JackieController>()->_stats._dir);
    }
}

IMPL_RTTI(JackieRenderController, EntityRenderController)
IMPL_EntityController_create(JackieRenderController, EntityRenderController)

std::string JackieRenderController::getTextureMapping()
{
    JackieController* ec = _entity->controller<JackieController>();
    
    switch (ec->_stats._dir)
    {
        case PDIR_UP:
            return "JACKIE_UP";
        case PDIR_LEFT:
        case PDIR_RIGHT:
            return "JACKIE_LEFT";
        case PDIR_DOWN:
            return "JACKIE_DOWN";
    }
    
    return EntityRenderController::getTextureMapping();
}
