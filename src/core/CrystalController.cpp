//
//  CrystalController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/v/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "CrystalController.hpp"

#include "Entity.hpp"
#include <box2d/b2_math.h>

#include "GameInputState.hpp"
#include "Rektangle.hpp"

#include "World.hpp"
#include "Macros.hpp"
#include "Constants.hpp"
#include "Timing.hpp"
#include "StringUtil.hpp"
#include "MathUtil.hpp"
#include "NetworkManagerServer.hpp"
#include "NetworkManagerClient.hpp"
#include "GameInputManager.hpp"
#include "GowAudioEngine.hpp"
#include "SoundUtil.hpp"
#include "Config.hpp"
#include "MainConfig.hpp"
#include "GameEngineState.hpp"
#include "Macros.hpp"
#include "Server.hpp"
#include "PlayerController.hpp"

IMPL_RTTI(CrystalController, EntityController);
IMPL_EntityController_create(CrystalController);

CrystalController::CrystalController(Entity* e) : EntityController(e)
{
    // Empty
}

void CrystalController::update()
{
    if (!_entity->getNetworkController()->isServer())
    {
        return;
    }
    
    if (_entity->state()._state == STAT_EXPLODING)
    {
        if (_entity->state()._stateTime >= 42)
        {
            _entity->requestDeletion();
        }
    }
}

void CrystalController::onMessage(uint16_t message, void* data)
{
    // TODO
    
    switch (message)
    {
        case MSG_ENCOUNTER:
        {
            if (_entity->state()._state != STAT_EXPLODING)
            {
                _entity->state()._state = STAT_EXPLODING;
                _entity->state()._stateTime = 0;
            }
            break;
        }
        default:
            break;
    }
}

float CrystalController::getWidthForRender()
{
    return _entity->state()._state == STAT_IDLE ? 4 : 8;
}

enum PushDirection
{
    PUIR_UP      = 0,
    PUIR_DOWN    = 1,
    PUIR_LEFT    = 2,
    PUIR_RIGHT   = 3
};

void CrystalController::push(int dir)
{
    if (dir == PUIR_UP)
    {
        _entity->pose()._velocity.x = 0;
        _entity->pose()._velocity.y = 1 * (rand() % 12);
        _entity->pose()._position.y += 0.5f;
    }
    else if (dir == PUIR_DOWN)
    {
        _entity->pose()._velocity.x = 0;
        _entity->pose()._velocity.y = -1 * (rand() % 12);
        _entity->pose()._position.y -= 0.5f;
    }
    else if (dir == PUIR_LEFT)
    {
        _entity->pose()._position.x -= 0.5f;
        _entity->pose()._velocity.x = -1 * (rand() % 12);
        _entity->pose()._velocity.y = 0;
    }
    else if (dir == PUIR_RIGHT)
    {
        _entity->pose()._position.x += 0.5f;
        _entity->pose()._velocity.x = 1 * (rand() % 12);
        _entity->pose()._velocity.y = 0;
    }
}

#include "InputMemoryBitStream.hpp"
#include "OutputMemoryBitStream.hpp"

IMPL_EntityNetworkController_create(CrystalNetworkController);

CrystalNetworkController::CrystalNetworkController(Entity* e, bool isServer) : EntityNetworkController(e, isServer), _controller(static_cast<CrystalController*>(e->getController()))
{
    // Empty
}

void CrystalNetworkController::read(InputMemoryBitStream& ip)
{
    uint8_t fromState = _entity->stateNetworkCache()._state;
    
    EntityNetworkController::read(ip);
    
    CrystalController& c = *_controller;
    
    SoundUtil::handleSound(_entity, fromState, _entity->state()._state);
}

uint16_t CrystalNetworkController::write(OutputMemoryBitStream& op, uint16_t dirtyState)
{
    uint16_t writtenState = EntityNetworkController::write(op, dirtyState);
    
    CrystalController& c = *_controller;
    
    return writtenState;
}

void CrystalNetworkController::recallNetworkCache()
{
    EntityNetworkController::recallNetworkCache();
    
    CrystalController& c = *_controller;
}

uint16_t CrystalNetworkController::getDirtyState()
{
    uint16_t ret = EntityNetworkController::getDirtyState();
    
    CrystalController& c = *_controller;
    
    return ret;
}
