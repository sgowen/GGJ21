//
//  HideAvatarController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

#include <assert.h>

IMPL_RTTI(HideAvatarController, EntityController)
IMPL_EntityController_create(HideAvatarController, EntityController)

void HideAvatarController::update(Entity* hide)
{
    uint8_t& state = hide->state()._state;
    uint16_t& stateTime = hide->state()._stateTime;
    
    if (state == STAT_SWING)
    {
        if (stateTime >= 42)
        {
            state = STAT_IDLE;
            
            World& w = hide->isServer() ? ENGINE_STATE_GAME_SRVR.getWorld() : ENGINE_STATE_GAME_CLNT.getWorld();
            for (Entity* e : w.getNetworkEntities())
            {
                if (e->controller()->getRTTI().isDerivedFrom(MonsterController::rtti) &&
                    e->dataField("isInEncounter").valueBool())
                {
                    e->requestDeletion();
                    hide->dataField("isInEncounter").valueBool() = false;
                    break;
                }
            }
        }
    }
    
    _entity->state()._state = state;
    _entity->state()._stateTime = stateTime;
}

void HideAvatarController::processInput(Entity* hide, InputState::PlayerInputState* pis, bool isLive)
{
    uint8_t& state = hide->state()._state;
    uint16_t& stateTime = hide->state()._stateTime;
    uint8_t piss = pis->_inputState;
    if (state == STAT_IDLE)
    {
        if (IS_BIT_SET(piss, GISF_CONFIRM))
        {
            state = STAT_SWING;
            stateTime = 0;
            
            if (isLive)
            {
                AUDIO_ENGINE.playSound(hide->renderController()->getSoundMapping(4));
            }
        }
    }
    else
    {
        if (IS_BIT_SET(piss, GISF_CANCEL))
        {
            state = STAT_IDLE;
            stateTime = 0;
        }
    }
}
