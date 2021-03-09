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

void HideAvatarController::update()
{
    uint8_t& state = _entity->state()._state;
    uint16_t& stateTime = _entity->state()._stateTime;
    
    if (state == STAT_SWING)
    {
        if (stateTime >= 60)
        {
            state = STAT_IDLE;
            
            World& w = _entity->isServer() ? ENGINE_STATE_GAME_SRVR.getWorld() : ENGINE_STATE_GAME_CLNT.getWorld();
            for (Entity* e : w.getNetworkEntities())
            {
                if (e->controller()->getRTTI().isDerivedFrom(MonsterController::rtti) &&
                    e->dataField("isInEncounter").valueBool())
                {
                    e->requestDeletion();
                    _entity->dataField("isInEncounter").valueBool() = false;
                    break;
                }
            }
        }
    }
}

void HideAvatarController::onMessage(uint16_t message)
{
    // TODO, handle battle messages
}

void HideAvatarController::processInput(InputState::PlayerInputState* pis, bool isLive)
{
    uint8_t& state = _entity->state()._state;
    uint16_t& stateTime = _entity->state()._stateTime;
    uint8_t piss = pis->_inputState;
    if (state == STAT_IDLE)
    {
        if (IS_BIT_SET(piss, GISF_CONFIRM))
        {
            state = STAT_SWING;
            stateTime = 0;
            
            if (isLive)
            {
                AUDIO_ENGINE.playSound(_entity->renderController()->getSoundMapping(4));
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
