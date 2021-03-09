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
IMPL_EntityController_create(HideController, EntityController)

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
    if (_entity->dataField("isInEncounter").valueBool())
    {
        _battleAvatar->update();
    }
    
    if (_entity->dataField("health").valueUInt16() == 0)
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
            bool& isInEncounter = _entity->dataField("isInEncounter").valueBool();
            if (!isInEncounter)
            {
                isInEncounter = true;
                _battleAvatar->state()._stateTime = 0;
                _entity->state()._state = STAT_IDLE;
                _entity->pose()._velocity.reset();
            }
            break;
        }
        default:
            break;
    }
}

void HideController::processInput(InputState* is, bool isLive)
{
    uint8_t playerID = _entity->metadata().getUInt("playerID");
    InputState::PlayerInputState* pis = is->playerInputStateForID(playerID);
    if (pis == NULL)
    {
        return;
    }
    
    if (_entity->dataField("isInEncounter").valueBool())
    {
        _battleAvatar->controller<HideAvatarController>()->processInput(pis, isLive);
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
