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
#include "GowAudioEngine.hpp"
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
_statsCache(_stats)
{
    // Empty
}

void MonsterController::update()
{
    World& w = _entity->isServer() ? ENGINE_STATE_GAME_SRVR.getWorld() : ENGINE_STATE_GAME_CLNT.getWorld();
    
    bool hasTarget = false;
    Vector2 playerPosition;
    std::vector<Entity*>& players = w.getPlayers();
    for (Entity* e : players)
    {
        PlayerController* ec = e->controller<PlayerController>();
        uint8_t playerID = ec->getPlayerID();
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
    }
}

#include "InputMemoryBitStream.hpp"
#include "OutputMemoryBitStream.hpp"

IMPL_EntityController_create(MonsterNetworkController, EntityNetworkController)

void MonsterNetworkController::read(InputMemoryBitStream& imbs)
{
    uint8_t fromState = _entity->stateCache()._state;
    
    EntityNetworkController::read(imbs);
    
    MonsterController* c = _entity->controller<MonsterController>();
    
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

uint8_t MonsterNetworkController::write(OutputMemoryBitStream& ombs, uint8_t dirtyState)
{
    uint8_t ret = EntityNetworkController::write(ombs, dirtyState);
    
    MonsterController* c = _entity->controller<MonsterController>();
    
    bool stats = IS_BIT_SET(dirtyState, MonsterController::RSTF_STATS);
    ombs.write(stats);
    if (stats)
    {
        ombs.write(c->_stats._health);
        ombs.write(c->_stats._dir);
        
        ret |= MonsterController::RSTF_STATS;
    }
    
    return ret;
}

void MonsterNetworkController::recallCache()
{
    EntityNetworkController::recallCache();
    
    MonsterController* c = _entity->controller<MonsterController>();
    
    c->_stats = c->_statsCache;
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
    
    return ret;
}

IMPL_RTTI(MonsterPhysicsController, TopDownPhysicsController)
IMPL_EntityController_create(MonsterPhysicsController, EntityPhysicsController)

void MonsterPhysicsController::onCollision(Entity* e)
{
    if (e->controller()->getRTTI().isDerivedFrom(HideController::rtti))
    {
        e->message(MSG_ENCOUNTER);
    }
}

#include "Assets.hpp"
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
    TextureRegion tr = ASSETS.findTextureRegion(getTextureMappingForEncounter(), 0);
    sb.addSprite(tr, CFG_MAIN._monsterBattleX, CFG_MAIN._monsterBattleY, CFG_MAIN._monsterBattleWidth, CFG_MAIN._monsterBattleHeight, 0);
}

std::string MonsterRenderController::getTextureMappingForEncounter()
{
    return "BIG_WITCH_PRETTY";
}
