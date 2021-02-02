//
//  MonsterController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/v/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "MonsterController.hpp"

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

IMPL_RTTI(MonsterController, EntityController);
IMPL_EntityController_create(MonsterController);

MonsterController::MonsterController(Entity* e) : EntityController(e),
_stats(),
_statsNetworkCache(_stats)
{
    // Empty
}

void MonsterController::update()
{
    if (!_entity->getNetworkController()->isServer())
    {
        return;
    }
    
    World& w = Server::getInstance()->getWorld();
    
    bool hasTarget = false;
    Vector2 playerPosition;
    std::vector<Entity*>& players = w.getPlayers();
    for (Entity* e : players)
    {
        PlayerController* pc = static_cast<PlayerController*>(e->getController());
        uint8_t playerID = pc->getPlayerID();
        if (playerID == 1)
        {
            float distance = b2Distance(e->getPosition(), _entity->getPosition());
            if (distance < CFG_MAIN._monsterLineOfSight)
            {
                hasTarget = true;
                playerPosition.set(e->getPosition().x, e->getPosition().y);
            }
        }
    }
    
    uint8_t& state = _entity->state()._state;
    uint16_t& stateTime = _entity->state()._stateTime;
    
    state = STAT_IDLE;
    
    if (hasTarget)
    {
        float angle = playerPosition.sub(_entity->getPosition().x, _entity->getPosition().y).angle();
        float radians = DEGREES_TO_RADIANS(angle);
        _entity->pose()._velocity.Set(cosf(radians) * CFG_MAIN._monsterMaxTopDownSpeed, sinf(radians) * CFG_MAIN._monsterMaxTopDownSpeed);
        
        if (_entity->pose()._velocity.y < 0)
        {
            state = STAT_MOVING;
            _stats._dir = MDIR_DOWN;
        }
        if (_entity->pose()._velocity.y > 0)
        {
            state = STAT_MOVING;
            _stats._dir = MDIR_UP;
        }
    }
    else
    {
        stateTime = 0;
        _entity->pose()._velocity *= 0.86f;
    }
}

void MonsterController::receiveMessage(uint16_t message, void* data)
{
    // TODO
    
    switch (message)
    {
        default:
            break;
    }
}

std::string MonsterController::getTextureMapping(uint8_t state)
{
    switch (_stats._dir)
    {
        case MDIR_UP:
            return "GENERIC_MONSTER_UP";
        case MDIR_DOWN:
            return "GENERIC_MONSTER_DOWN";
        default:
            assert(false);
    }
}

void MonsterController::onCollision(Entity* e)
{
    if (!_entity->getNetworkController()->isServer())
    {
        return;
    }
    
    if (_entity->isRequestingDeletion())
    {
        return;
    }
    
    if (e->getController()->getRTTI().derivesFrom(PlayerController::rtti))
    {
        PlayerController* pc = static_cast<PlayerController*>(e->getController());
        
        int playerID = pc->getPlayerID();
        if (playerID == 1)
        {
            e->getController()->onMessage(MSG_ENCOUNTER);
        }
    }
}

std::string MonsterController::getTextureMappingForEncounter()
{
    return "BIG_WITCH_PRETTY";
}

#include "InputMemoryBitStream.hpp"
#include "OutputMemoryBitStream.hpp"

IMPL_EntityNetworkController_create(MonsterNetworkController);

MonsterNetworkController::MonsterNetworkController(Entity* e, bool isServer) : EntityNetworkController(e, isServer), _controller(static_cast<MonsterController*>(e->getController()))
{
    // Empty
}

void MonsterNetworkController::read(InputMemoryBitStream& ip)
{
    uint8_t fromState = _entity->stateNetworkCache()._state;
    
    EntityNetworkController::read(ip);
    
    MonsterController& c = *_controller;
    
    bool stateBit;
    
    ip.read(stateBit);
    if (stateBit)
    {
        ip.read(c._stats._health);
        ip.read(c._stats._dir);
        
        c._statsNetworkCache = c._stats;
    }
    
    SoundUtil::handleSound(_entity, fromState, _entity->state()._state);
}

uint16_t MonsterNetworkController::write(OutputMemoryBitStream& op, uint16_t dirtyState)
{
    uint16_t writtenState = EntityNetworkController::write(op, dirtyState);
    
    MonsterController& c = *_controller;
    
    bool stats = IS_BIT_SET(dirtyState, MonsterController::RSTF_STATS);
    op.write(stats);
    if (stats)
    {
        op.write(c._stats._health);
        op.write(c._stats._dir);
        
        writtenState |= MonsterController::RSTF_STATS;
    }
    
    return writtenState;
}

void MonsterNetworkController::recallNetworkCache()
{
    EntityNetworkController::recallNetworkCache();
    
    MonsterController& c = *_controller;
    
    c._stats = c._statsNetworkCache;
}

uint16_t MonsterNetworkController::getDirtyState()
{
    uint16_t ret = EntityNetworkController::getDirtyState();
    
    MonsterController& c = *_controller;
    
    if (c._statsNetworkCache != c._stats)
    {
        c._statsNetworkCache = c._stats;
        ret |= MonsterController::RSTF_STATS;
    }
    
    return ret;
}
