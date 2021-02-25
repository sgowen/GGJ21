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
#include "NetworkServer.hpp"
#include "NetworkClient.hpp"
#include "GameInputManager.hpp"
#include "AudioEngineFactory.hpp"
#include "SoundUtil.hpp"
#include "Config.hpp"
#include "MainConfig.hpp"
#include "Macros.hpp"
#include "GameServerEngineState.hpp"
#include "GameClientEngineState.hpp"
#include "HideController.hpp"
#include "MathUtil.hpp"

IMPL_RTTI(MonsterController, EntityController)
IMPL_EntityController_create(MonsterController, EntityController)

MonsterController::MonsterController(Entity* e) : EntityController(e),
_stats(),
_statsCache(_stats),
_encounter(),
_encounterCache(_encounter)
{
    // Empty
}

void MonsterController::update()
{
    bool hasTarget = false;
    Vector2 playerPosition;
    uint8_t& state = _entity->state()._state;
    uint16_t& stateTime = _entity->state()._stateTime;
    state = STAT_IDLE;
    
    if (_encounter._isInCounter)
    {
        ++_encounter._stateTime;
    }
    else
    {
        World& w = _entity->isServer() ? ENGINE_STATE_GAME_SRVR.getWorld() : ENGINE_STATE_GAME_CLNT.getWorld();
        std::vector<Entity*>& players = w.getPlayers();
        for (Entity* e : players)
        {
            PlayerController* ec = e->controller<PlayerController>();
            uint8_t playerID = ec->getPlayerID();
            if (playerID == 1)
            {
                float distance = e->position().dist(_entity->position());
                if (distance < CFG_MAIN._monsterLineOfSight)
                {
                    hasTarget = true;
                    playerPosition.set(e->position()._x, e->position()._y);
                }
            }
        }
    }
    
    if (hasTarget)
    {
        float angle = playerPosition.sub(_entity->position()._x, _entity->position()._y).angle();
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
    }
}

bool MonsterController::isInEncounter()
{
    return _encounter._isInCounter;
}

#include "InputMemoryBitStream.hpp"
#include "OutputMemoryBitStream.hpp"

IMPL_EntityController_create(MonsterNetworkController, EntityNetworkController)

void MonsterNetworkController::read(InputMemoryBitStream& imbs)
{
    uint8_t fromState = _entity->stateCache()._state;
    
    EntityNetworkController::read(imbs);
    
    MonsterController* ec = _entity->controller<MonsterController>();
    
    bool stateBit;
    
    imbs.read(stateBit);
    if (stateBit)
    {
        imbs.read(ec->_stats._health);
        imbs.read(ec->_stats._dir);
        
        ec->_statsCache = ec->_stats;
    }
    
    imbs.read(stateBit);
    if (stateBit)
    {
        imbs.read(ec->_encounter._isInCounter);
        imbs.read(ec->_encounter._state);
        imbs.read(ec->_encounter._stateTime);
        
        ec->_encounterCache = ec->_encounter;
    }
    
    SoundUtil::playSoundForStateIfChanged(_entity, fromState, _entity->state()._state);
}

uint8_t MonsterNetworkController::write(OutputMemoryBitStream& ombs, uint8_t dirtyState)
{
    uint8_t ret = EntityNetworkController::write(ombs, dirtyState);
    
    MonsterController* ec = _entity->controller<MonsterController>();
    
    bool stats = IS_BIT_SET(dirtyState, MonsterController::RSTF_STATS);
    ombs.write(stats);
    if (stats)
    {
        ombs.write(ec->_stats._health);
        ombs.write(ec->_stats._dir);
        
        ret |= MonsterController::RSTF_STATS;
    }
    
    bool RSTF_ENCOUNTER = IS_BIT_SET(dirtyState, MonsterController::RSTF_ENCOUNTER);
    ombs.write(RSTF_ENCOUNTER);
    if (RSTF_ENCOUNTER)
    {
        ombs.write(ec->_encounter._isInCounter);
        ombs.write(ec->_encounter._state);
        ombs.write(ec->_encounter._stateTime);
        
        ret |= MonsterController::RSTF_ENCOUNTER;
    }
    
    return ret;
}

void MonsterNetworkController::recallCache()
{
    EntityNetworkController::recallCache();
    
    MonsterController* c = _entity->controller<MonsterController>();
    
    c->_stats = c->_statsCache;
    c->_encounter = c->_encounterCache;
}

uint8_t MonsterNetworkController::refreshDirtyState()
{
    uint8_t ret = EntityNetworkController::refreshDirtyState();
    
    MonsterController* c = _entity->controller<MonsterController>();
    
    if (c->_statsCache != c->_stats)
    {
        c->_statsCache = c->_stats;
        ret |= MonsterController::RSTF_STATS;
    }
    
    if (c->_encounterCache != c->_encounter)
    {
        c->_encounterCache = c->_encounter;
        ret |= MonsterController::RSTF_ENCOUNTER;
    }
    
    return ret;
}

IMPL_RTTI(MonsterPhysicsController, TopDownPhysicsController)
IMPL_EntityController_create(MonsterPhysicsController, EntityPhysicsController)

void MonsterPhysicsController::onCollision(Entity* e)
{
    if (e->controller()->getRTTI().isDerivedFrom(HideController::rtti))
    {
        e->message(MSG_ENCOUNTER);
        
        MonsterController* ec = _entity->controller<MonsterController>();
        ec->_encounter._isInCounter = true;
        ec->_encounter._stateTime = 0;
        _entity->state()._state = MonsterController::STAT_IDLE;
        _entity->pose()._velocity._x = 0;
        _entity->pose()._velocity._y = 0;
    }
}

#include "AssetManager.hpp"
#include "SpriteBatcher.hpp"

IMPL_RTTI(MonsterRenderController, EntityRenderController)
IMPL_EntityController_create(MonsterRenderController, EntityRenderController)

std::string MonsterRenderController::getTextureMapping()
{
    MonsterController* ec = _entity->controller<MonsterController>();
    
    switch (ec->_stats._dir)
    {
        case MDIR_UP:
            return "GENERIC_MONSTER_UP";
        case MDIR_DOWN:
            return "GENERIC_MONSTER_DOWN";
    }
    
    return EntityRenderController::getTextureMapping();
}

void MonsterRenderController::addSpriteForEncounter(SpriteBatcher& sb)
{
    TextureRegion tr = ASSETS.textureRegion(getTextureMappingForEncounter(), 0);
    sb.addSprite(tr, CFG_MAIN._monsterBattleX, CFG_MAIN._monsterBattleY, CFG_MAIN._monsterBattleWidth, CFG_MAIN._monsterBattleHeight, 0);
}

std::string MonsterRenderController::getTextureMappingForEncounter()
{
    return "BIG_WITCH_PRETTY";
}
