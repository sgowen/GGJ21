//
//  PlayerController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "PlayerController.hpp"

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
#include "MonsterController.hpp"
#include "Server.hpp"

IMPL_RTTI(PlayerController, EntityController);
IMPL_EntityController_create(PlayerController);

PlayerController::PlayerController(Entity* e) : EntityController(e),
_playerInfo(),
_playerInfoNetworkCache(_playerInfo),
_stats(),
_statsNetworkCache(_stats),
_encounter(),
_encounterNetworkCache(_encounter)
{
    // Empty
}

void PlayerController::update()
{
    uint8_t& state = _entity->state()._state;
    uint16_t& stateTime = _entity->state()._stateTime;
    
    ++_encounter._stateTime;
    
    if (!_entity->getNetworkController()->isServer())
    {
        return;
    }
    
    if (_encounter._state == ESTA_SWING)
    {
        if (_encounter._stateTime >= 42)
        {
            _encounter._state = ESTA_IDLE;
            
            World& w = Server::getInstance()->getWorld();
            for (Entity* e : w.getDynamicEntities())
            {
                if (e->getController()->getRTTI().derivesFrom(MonsterController::rtti))
                {
                    e->requestDeletion();
                    _encounter._isInCounter = false;
                    break;
                }
            }
        }
    }
    
    if (_stats._health == 0)
    {
        _entity->requestDeletion();
    }
}

void PlayerController::onMessage(uint16_t message, void* data)
{
    // TODO
    
    switch (message)
    {
        case MSG_ENCOUNTER:
        {
            _encounter._isInCounter = true;
            _entity->state()._state = STAT_IDLE;
            _entity->pose()._velocity.x = 0;
            _entity->pose()._velocity.y = 0;
            break;
        }
        default:
            break;
    }
}

void PlayerController::processInput(InputState* inputState)
{
    GameInputState* is = static_cast<GameInputState*>(inputState);
    uint8_t playerID = getPlayerID();
    GameInputState::PlayerInputState* playerInputState = is->getPlayerInputStateForID(playerID);
    if (playerInputState == NULL)
    {
        return;
    }
    
    if (_encounter._isInCounter)
    {
        if (_encounter._state == ESTA_IDLE)
        {
            uint8_t inpt = playerInputState->getInputState();
            if (IS_BIT_SET(inpt, GISF_MOVING_RIGHT))
            {
                _encounter._state = ESTA_SWING;
                _encounter._stateTime = 0;
                
                if (ENGINE_STATE_GAME.isLive())
                {
                    GOW_AUDIO.playSound(_entity->getSoundMapping(4));
                }
            }
        }
    }
    else
    {
        uint8_t fromState = _entity->state()._state;
        uint8_t& state = _entity->state()._state;
        uint16_t& stateTime = _entity->state()._stateTime;
        uint8_t inpt = playerInputState->getInputState();
        
        state = STAT_IDLE;
        const b2Vec2& vel = _entity->getVelocity();
        float desiredVel[2] = { 0.0f, 0.0f };
        float maxSpeed = CFG_MAIN._playerMaxTopDownSpeed;
        float maxSpeedHalved = maxSpeed / 2;
        if (IS_BIT_SET(inpt, GISF_MOVING_UP))
        {
            state = STAT_MOVING;
            _stats._dir = PDIR_UP;
            desiredVel[1] = b2Min(vel.y + maxSpeedHalved, maxSpeed);
        }
        if (IS_BIT_SET(inpt, GISF_MOVING_LEFT))
        {
            state = STAT_MOVING;
            _stats._dir = PDIR_LEFT;
            desiredVel[0] = b2Max(vel.x - maxSpeedHalved, -maxSpeed);
        }
        if (IS_BIT_SET(inpt, GISF_MOVING_DOWN))
        {
            state = STAT_MOVING;
            _stats._dir = PDIR_DOWN;
            desiredVel[1] = b2Max(vel.y - maxSpeedHalved, -maxSpeed);
        }
        if (IS_BIT_SET(inpt, GISF_MOVING_RIGHT))
        {
            state = STAT_MOVING;
            _stats._dir = PDIR_RIGHT;
            desiredVel[0] = b2Min(vel.x + maxSpeedHalved, maxSpeed);
        }
        
        if (state == STAT_IDLE)
        {
            stateTime = 6; // 2nd frame is more appropriate for idle
            desiredVel[0] = vel.x * 0.86f;
            desiredVel[1] = vel.y * 0.86f;
        }
        
        _entity->pose()._velocity.x = desiredVel[0];
        _entity->pose()._velocity.y = desiredVel[1];
        
        if (ENGINE_STATE_GAME.isLive())
        {
            SoundUtil::handleSound(_entity, fromState, state);
        }
    }
}

void PlayerController::enforceBounds(Rektangle& bounds)
{
    float x = _entity->getPosition().x;
    float y = _entity->getPosition().y;
    
    if (_entity->getPosition().x > bounds.right())
    {
        _entity->setPosition(b2Vec2(bounds.right(), y));
    }
    else if (_entity->getPosition().x < bounds.left())
    {
        _entity->setPosition(b2Vec2(bounds.left(), y));
    }
    
    if (_entity->getPosition().y > bounds.top())
    {
        _entity->setPosition(b2Vec2(x, bounds.top()));
    }
    else if (_entity->getPosition().y < bounds.bottom())
    {
        _entity->setPosition(b2Vec2(x, bounds.bottom()));
    }
}

bool PlayerController::isInEncounter()
{
    return _encounter._isInCounter;
}

uint16_t PlayerController::encounterStateTime()
{
    return _encounter._stateTime;
}

void PlayerController::setAddressHash(uint64_t inValue)
{
    _playerInfo._addressHash = inValue;
}

uint64_t PlayerController::getAddressHash() const
{
    return _playerInfo._addressHash;
}

void PlayerController::setPlayerID(uint8_t inValue)
{
    _playerInfo._playerID = inValue;
}

uint8_t PlayerController::getPlayerID() const
{
    return _playerInfo._playerID;
}

void PlayerController::setPlayerName(std::string inValue)
{
    _playerInfo._playerName = inValue;
}

std::string& PlayerController::getPlayerName()
{
    return _playerInfo._playerName;
}

PlayerDirection PlayerController::getPlayerDirection()
{
    return static_cast<PlayerDirection>(_stats._dir);
}

uint16_t PlayerController::getHealth()
{
    return _stats._health;
}

#include "InputMemoryBitStream.hpp"
#include "OutputMemoryBitStream.hpp"

IMPL_EntityNetworkController_create(PlayerNetworkController);

PlayerNetworkController::PlayerNetworkController(Entity* e, bool isServer) : EntityNetworkController(e, isServer), _controller(static_cast<PlayerController*>(e->getController())), _isLocalPlayer(false)
{
    // Empty
}

void PlayerNetworkController::read(InputMemoryBitStream& ip)
{
    uint8_t fromState = _entity->stateNetworkCache()._state;
    
    EntityNetworkController::read(ip);
    
    PlayerController& c = *_controller;
    
    bool stateBit;
    
    ip.read(stateBit);
    if (stateBit)
    {
        ip.read(c._playerInfo._addressHash);
        ip.read<uint8_t, 3>(c._playerInfo._playerID);
        ip.readSmall(c._playerInfo._playerName);
        
        _isLocalPlayer = NW_MGR_CLIENT->isPlayerIDLocal(c._playerInfo._playerID);
        
        c._playerInfoNetworkCache = c._playerInfo;
    }
    
    ip.read(stateBit);
    if (stateBit)
    {
        ip.read(c._stats._health);
        ip.read(c._stats._dir);
        
        c._statsNetworkCache = c._stats;
    }
    
    ip.read(stateBit);
    if (stateBit)
    {
        bool wasInCounter = c._encounter._isInCounter;
        
        ip.read(c._encounter._isInCounter);
        ip.read(c._encounter._stateTime);
        ip.read(c._encounter._state);
        
        c._encounterNetworkCache = c._encounter;
        
        if (!wasInCounter && c._encounter._isInCounter)
        {
            GOW_AUDIO.playSound(_entity->getSoundMapping(2));
        }
        else if (wasInCounter && !c._encounter._isInCounter)
        {
            GOW_AUDIO.playSound(_entity->getSoundMapping(3));
        }
    }
    
    if (!_isLocalPlayer)
    {
        SoundUtil::handleSound(_entity, fromState, _entity->state()._state);
    }
}

uint16_t PlayerNetworkController::write(OutputMemoryBitStream& op, uint16_t dirtyState)
{
    uint16_t writtenState = EntityNetworkController::write(op, dirtyState);
    
    PlayerController& c = *_controller;
    
    bool playerInfo = IS_BIT_SET(dirtyState, PlayerController::RSTF_PLAYER_INFO);
    op.write(playerInfo);
    if (playerInfo)
    {
        op.write(c._playerInfo._addressHash);
        op.write<uint8_t, 3>(c._playerInfo._playerID);
        op.writeSmall(c._playerInfo._playerName);
        
        writtenState |= PlayerController::RSTF_PLAYER_INFO;
    }
    
    bool stats = IS_BIT_SET(dirtyState, PlayerController::RSTF_STATS);
    op.write(stats);
    if (stats)
    {
        op.write(c._stats._health);
        op.write(c._stats._dir);
        
        writtenState |= PlayerController::RSTF_STATS;
    }
    
    bool encounter = IS_BIT_SET(dirtyState, PlayerController::RSTF_ENCOUNTER);
    op.write(encounter);
    if (encounter)
    {
        op.write(c._encounter._isInCounter);
        op.write(c._encounter._stateTime);
        op.write(c._encounter._state);
        
        writtenState |= PlayerController::RSTF_ENCOUNTER;
    }
    
    return writtenState;
}

void PlayerNetworkController::recallNetworkCache()
{
    EntityNetworkController::recallNetworkCache();
    
    PlayerController& c = *_controller;
    
    c._playerInfo = c._playerInfoNetworkCache;
    c._stats = c._statsNetworkCache;
    c._encounter = c._encounterNetworkCache;
}

uint16_t PlayerNetworkController::getDirtyState()
{
    uint16_t ret = EntityNetworkController::getDirtyState();
    
    PlayerController& c = *_controller;
    
    if (c._playerInfoNetworkCache != c._playerInfo)
    {
        c._playerInfoNetworkCache = c._playerInfo;
        ret |= PlayerController::RSTF_PLAYER_INFO;
    }
    
    if (c._statsNetworkCache != c._stats)
    {
        c._statsNetworkCache = c._stats;
        ret |= PlayerController::RSTF_STATS;
    }
    
    if (c._encounterNetworkCache != c._encounter)
    {
        c._encounterNetworkCache = c._encounter;
        ret |= PlayerController::RSTF_ENCOUNTER;
    }
    
    return ret;
}

bool PlayerNetworkController::isLocalPlayer()
{
    return _isLocalPlayer;
}
