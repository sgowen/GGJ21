//
//  OvenController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/v/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "OvenController.hpp"

#include "Entity.hpp"
#include <box2d/b2_math.h>

#include "GameInputState.hpp"
#include "Rektangle.hpp"

#include "World.hpp"
#include "Macros.hpp"
#include "TimeTracker.hpp"
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
#include "CrystalController.hpp"

IMPL_RTTI(OvenController, EntityController);
IMPL_EntityController_create(OvenController);

OvenController::OvenController(Entity* e) : EntityController(e)
{
    // Empty
}

void OvenController::onCollision(Entity* e)
{
    if (!_entity->getNetworkController()->isServer())
    {
        return;
    }
    
    if (e->getController()->getRTTI().derivesFrom(CrystalController::rtti))
    {
        CrystalController* pc = static_cast<CrystalController*>(e->getController());
        
        e->getController()->onMessage(MSG_ENCOUNTER);
    }
}

#include "InputMemoryBitStream.hpp"
#include "OutputMemoryBitStream.hpp"

IMPL_EntityNetworkController_create(OvenNetworkController);

OvenNetworkController::OvenNetworkController(Entity* e, bool isServer) : EntityNetworkController(e, isServer), _controller(static_cast<OvenController*>(e->getController()))
{
    // Empty
}

void OvenNetworkController::read(InputMemoryBitStream& ip)
{
    uint8_t fromState = _entity->stateNetworkCache()._state;
    
    EntityNetworkController::read(ip);
    
    OvenController& c = *_controller;
    
    SoundUtil::handleSound(_entity, fromState, _entity->state()._state);
}

uint16_t OvenNetworkController::write(OutputMemoryBitStream& op, uint16_t dirtyState)
{
    uint16_t writtenState = EntityNetworkController::write(op, dirtyState);
    
    OvenController& c = *_controller;
    
    return writtenState;
}

void OvenNetworkController::recallNetworkCache()
{
    EntityNetworkController::recallNetworkCache();
    
    OvenController& c = *_controller;
}

uint16_t OvenNetworkController::getDirtyState()
{
    uint16_t ret = EntityNetworkController::getDirtyState();
    
    OvenController& c = *_controller;
    
    return ret;
}
