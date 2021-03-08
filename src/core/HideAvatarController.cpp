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
IMPL_EntityController_create(HideAvatarController)

HideAvatarController::HideAvatarController(Entity* e) : EntityController(e)
{
    // Empty
}

void HideAvatarController::update()
{
    uint8_t& state = _entity->state()._state;
    uint16_t& stateTime = _entity->state()._stateTime;
    uint8_t& stateFlags = _entity->state()._stateFlags;
    
    if (state == ESTA_SWING)
    {
        if (stateTime >= 42)
        {
            state = ESTA_IDLE;
            
            World& w = _entity->isServer() ? ENGINE_STATE_GAME_SRVR.getWorld() : ENGINE_STATE_GAME_CLNT.getWorld();
            for (Entity* e : w.getNetworkEntities())
            {
                if (e->controller()->getRTTI().isDerivedFrom(MonsterController::rtti) &&
                    e->nwDataField("isInCounter").valueBool())
                {
                    e->requestDeletion();
                    _entity->nwDataField("isInCounter").valueBool() = false;
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
    uint8_t& stateFlags = _entity->state()._stateFlags;
    uint8_t piss = pis->_inputState;
    if (state == ESTA_IDLE)
    {
        if (IS_BIT_SET(piss, GISF_CONFIRM))
        {
            state = ESTA_SWING;
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
            state = ESTA_IDLE;
            stateTime = 0;
        }
    }
}
