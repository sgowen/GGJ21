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
                    e->controller<MonsterController>()->isInEncounter())
                {
                    e->requestDeletion();
                    _encounter._isInCounter = false;
                    break;
                }
            }
        }
    }
    
    if (_stats._health == 0)
    {
        _entity->requestDeletion();
    }
}

void HideAvatarController::onMessage(uint16_t message)
{
    // TODO, handle battle messages
}

void HideAvatarController::processInput(InputState* is, bool isLive)
{
    uint8_t playerID = _entity->entityDef()._data.getUInt("playerID");
    InputState::PlayerInputState* pis = is->playerInputStateForID(playerID);
    if (pis == NULL)
    {
        return;
    }
    
    uint8_t piss = pis->_inputState;
    if (_encounter._state == ESTA_IDLE)
    {
        if (IS_BIT_SET(piss, GISF_CONFIRM))
        {
            _encounter._state = ESTA_SWING;
            _encounter._stateTime = 0;
            
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
            _encounter._state = ESTA_IDLE;
            _encounter._stateTime = 0;
        }
    }
}
