//
//  MonsterController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/v/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "MonsterController.hpp"

#include "Entity.hpp"

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
#include "HidePlayerController.hpp"

IMPL_RTTI(MonsterController, EntityController)
IMPL_EntityController_create(MonsterController)

MonsterController::MonsterController(Entity* e) : EntityController(e),
_stats(),
_statsCache(_stats)
{
    // Empty
}

void MonsterController::update()
{
    if (!_entity->networkController()->isServer())
    {
        return;
    }
    
    World& w = Server::getInstance()->getWorld();
    
    bool hasTarget = false;
    Vector2 playerPosition;
    std::vector<Entity*>& players = w.getPlayers();
    for (Entity* e : players)
    {
        PlayerController* c = static_cast<PlayerController*>(e->controller());
        uint8_t playerID = c->getPlayerID();
        if (playerID == 1)
        {
            float distance = e->getPosition().dist(_entity->getPosition());
            if (distance < CFG_MAIN._monsterLineOfSight)
            {
                hasTarget = true;
                playerPosition.set(e->getPosition()._x, e->getPosition()._y);
            }
        }
    }
    
    uint8_t& state = _entity->state()._state;
    uint16_t& stateTime = _entity->state()._stateTime;
    
    state = STAT_IDLE;
    
    if (hasTarget)
    {
        float angle = playerPosition.sub(_entity->getPosition()._x, _entity->getPosition()._y).angle();
        float radians = DEGREES_TO_RADIANS(angle);
        _entity->pose()._velocity.set(cosf(radians) * CFG_MAIN._monsterMaxTopDownSpeed, sinf(radians) * CFG_MAIN._monsterMaxTopDownSpeed);
        
        if (_entity->pose()._velocity._y < 0)
        {
            state = STAT_MOVING;
            _stats._dir = MDIR_DOWN;
        }
        if (_entity->pose()._velocity._y > 0)
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

std::string MonsterController::getTextureMapping()
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
    if (e->controller()->getRTTI().isDerivedFrom(HidePlayerController::rtti))
    {
        HidePlayerController* c = static_cast<HidePlayerController*>(e->controller());
        c->onMessage(MSG_ENCOUNTER);
    }
}

std::string MonsterController::getTextureMappingForEncounter()
{
    return "BIG_WITCH_PRETTY";
}

#include "InputMemoryBitStream.hpp"
#include "OutputMemoryBitStream.hpp"

IMPL_EntityNetworkController_create(MonsterNetworkController)

void MonsterNetworkController::read(InputMemoryBitStream& imbs)
{
    uint8_t fromState = _entity->stateCache()._state;
    
    EntityNetworkController::read(imbs);
    
    MonsterController* c = static_cast<MonsterController*>(_entity->controller());
    
    bool stateBit;
    
    imbs.read(stateBit);
    if (stateBit)
    {
        imbs.read(c->_stats._health);
        imbs.read(c->_stats._dir);
        
        c->_statsCache = c->_stats;
    }
    
    SoundUtil::playSoundForStateIfChanged(_entity, fromState, _entity->state()._state);
}

uint16_t MonsterNetworkController::write(OutputMemoryBitStream& ombs, uint16_t dirtyState)
{
    uint16_t writtenState = EntityNetworkController::write(ombs, dirtyState);
    
    MonsterController* c = static_cast<MonsterController*>(_entity->controller());
    
    bool stats = IS_BIT_SET(dirtyState, MonsterController::RSTF_STATS);
    ombs.write(stats);
    if (stats)
    {
        ombs.write(c->_stats._health);
        ombs.write(c->_stats._dir);
        
        writtenState |= MonsterController::RSTF_STATS;
    }
    
    return writtenState;
}

void MonsterNetworkController::recallCache()
{
    EntityNetworkController::recallCache();
    
    MonsterController* c = static_cast<MonsterController*>(_entity->controller());
    
    c->_stats = c->_statsCache;
}

uint16_t MonsterNetworkController::refreshDirtyState()
{
    uint16_t ret = EntityNetworkController::refreshDirtyState();
    
    MonsterController* c = static_cast<MonsterController*>(_entity->controller());
    
    if (c->_statsCache != c->_stats)
    {
        c->_statsCache = c->_stats;
        ret |= MonsterController::RSTF_STATS;
    }
    
    return ret;
}
