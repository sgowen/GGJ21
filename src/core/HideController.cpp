//
//  HideController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

#include <assert.h>

IMPL_RTTI(HideController, PlayerController)
IMPL_EntityController_create(HideController)

HideController::HideController(Entity* e) : PlayerController(e),
_battleAvatar(ENTITY_MGR.createEntity(EntityInstanceDef(0, 'HIDA', CFG_MAIN.playerBattleX(), CFG_MAIN.playerBattleY(), e->isServer())))
{
    // Empty
}

HideController::~HideController()
{
    delete _battleAvatar;
}

void HideController::update()
{
    if (_encounter._isInCounter)
    {
        ++_encounter._stateTime;
        _battleAvatar->update();
    }
    
    if (_stats._health == 0)
    {
        _entity->requestDeletion();
    }
}

void HideController::onMessage(uint16_t message)
{
    switch (message)
    {
        case MSG_ENCOUNTER:
        {
            if (!_encounter._isInCounter)
            {
                _encounter._isInCounter = true;
                _encounter._stateTime = 0;
                _entity->state()._state = STAT_IDLE;
                _entity->pose()._velocity._x = 0;
                _entity->pose()._velocity._y = 0;
            }
            break;
        }
        default:
            break;
    }
}

void HideController::processInput(InputState* is, bool isLive)
{
    uint8_t playerID = _entity->entityDef()._data.getUInt("playerID");
    InputState::PlayerInputState* pis = is->playerInputStateForID(playerID);
    if (pis == NULL)
    {
        return;
    }
    
    if (_encounter._isInCounter)
    {
        _battleAvatar->processInput(pis, isLive);
    }
    else
    {
        PlayerController::processInput(is, isLive);
    }
}

Entity* HideController::battleAvatar()
{
    return _battleAvatar;
}
