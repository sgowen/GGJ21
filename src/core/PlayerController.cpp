//
//  PlayerController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

IMPL_RTTI(PlayerController, EntityController)
IMPL_EntityController_create(PlayerController, EntityController)

void PlayerController::processInput(InputState* is, bool isLive)
{
    Entity& e = *_entity;
    uint8_t playerID = e.metadata().getUInt("playerID");
    InputState::PlayerInputState* pis = is->playerInputStateForID(playerID);
    if (pis == NULL)
    {
        return;
    }
    
    uint8_t fromState = e.state()._state;
    uint8_t& state = e.state()._state;
    uint8_t& stateFlags = e.state()._stateFlags;
    uint8_t piss = pis->_inputState;
    
    state = STAT_IDLE;
    Vector2& vel = e.velocity();
    if (IS_BIT_SET(piss, GISF_MOVING_UP))
    {
        state = STAT_MOVING;
        stateFlags = EDIR_UP;
        vel._y = CFG_MAIN.playerMaxTopDownSpeed();
    }
    if (IS_BIT_SET(piss, GISF_MOVING_LEFT))
    {
        state = STAT_MOVING;
        stateFlags = EDIR_LEFT;
        vel._x = -  CFG_MAIN.playerMaxTopDownSpeed();
    }
    if (IS_BIT_SET(piss, GISF_MOVING_DOWN))
    {
        state = STAT_MOVING;
        stateFlags = EDIR_DOWN;
        vel._y = -CFG_MAIN.playerMaxTopDownSpeed();
    }
    if (IS_BIT_SET(piss, GISF_MOVING_RIGHT))
    {
        state = STAT_MOVING;
        stateFlags = EDIR_RIGHT;
        vel._x = CFG_MAIN.playerMaxTopDownSpeed();
    }
    
    e.pose()._isXFlipped = stateFlags == EDIR_RIGHT;
    
    if (isLive)
    {
        SoundUtil::playSoundForStateIfChanged(e, fromState, state);
    }
}
