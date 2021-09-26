//
//  HideController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

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
    if (_entity->dataField("isInBattle").valueBool())
    {
        uint8_t& state = _entity->dataField("battleState").valueUInt8();
        uint16_t& stateTime = _entity->dataField("battleStateTime").valueUInt16();
        
        ++stateTime;
        
        if (state == BSTT_SWING)
        {
            // TODO, this should be data-driven
            _battleAvatar->pose()._width = stateTime >= 14 && stateTime < 35 ? 20.0f : 16.0f;
            
            if (stateTime >= 42)
            {
                state = BSTT_IDLE;
                
                World& w = _entity->isServer() ? ENGINE_STATE_GAME_SRVR.world() : ENGINE_STATE_GAME_CLNT.world();
                for (Entity* e : w.getNetworkEntities())
                {
                    if (e->controller()->getRTTI().isDerivedFrom(MonsterController::rtti) &&
                        e->dataField("isInBattle").valueBool())
                    {
                        e->requestDeletion();
                        _entity->dataField("isInBattle").valueBool() = false;
                        break;
                    }
                }
            }
        }
        
        _battleAvatar->state()._state = state;
        _battleAvatar->state()._stateTime = stateTime;
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
            bool& isInBattle = _entity->dataField("isInBattle").valueBool();
            if (!isInBattle)
            {
                isInBattle = true;
                _entity->dataField("battleState").valueUInt8() = BSTT_IDLE;
                _entity->dataField("battleStateTime").valueUInt16() = 0;
                
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
    if (pis == nullptr)
    {
        return;
    }
    
    if (_entity->dataField("isInBattle").valueBool())
    {
        uint8_t& state = _entity->dataField("battleState").valueUInt8();
        uint16_t& stateTime = _entity->dataField("battleStateTime").valueUInt16();
        uint8_t piss = pis->_inputState;
        if (state == STAT_IDLE)
        {
            if (IS_BIT_SET(piss, GISF_CONFIRM))
            {
                state = BSTT_SWING;
                stateTime = 0;
            }
        }
        else
        {
            if (IS_BIT_SET(piss, GISF_CANCEL))
            {
                state = BSTT_IDLE;
                stateTime = 0;
            }
        }
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
