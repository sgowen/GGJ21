//
//  PlayerController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "PlayerController.hpp"

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
#include "MonsterController.hpp"
#include "Server.hpp"

IMPL_RTTI(PlayerController, EntityController)
IMPL_EntityController_create(PlayerController)

PlayerController::PlayerController(Entity* e) : EntityController(e),
_playerInfo(),
_playerInfoCache(_playerInfo),
_stats(),
_statsCache(_stats)
{
    // Empty
}

void PlayerController::update()
{
    if (_entity->state()._state == STAT_IDLE)
    {
        // 2nd frame is more appropriate for idle
        _entity->state()._stateTime = 6;
    }
}

void PlayerController::processInput(InputState* inputState, bool isLocal)
{
    GameInputState* is = static_cast<GameInputState*>(inputState);
    uint8_t playerID = getPlayerID();
    GameInputState::PlayerInputState* playerInputState = is->getPlayerInputStateForID(playerID);
    if (playerInputState == NULL)
    {
        return;
    }
    
    uint8_t fromState = _entity->state()._state;
    uint8_t& state = _entity->state()._state;
    uint8_t inpt = playerInputState->inputState();
    
    state = STAT_IDLE;
    const Vector2& vel = _entity->getVelocity();
    float desiredVel[2] = { 0.0f, 0.0f };
    float maxSpeed = CFG_MAIN._playerMaxTopDownSpeed;
    float maxSpeedHalved = maxSpeed / 2;
    if (IS_BIT_SET(inpt, GISF_MOVING_UP))
    {
        state = STAT_MOVING;
        _stats._dir = PDIR_UP;
        desiredVel[1] = MIN(vel._y + maxSpeedHalved, maxSpeed);
    }
    if (IS_BIT_SET(inpt, GISF_MOVING_LEFT))
    {
        state = STAT_MOVING;
        _stats._dir = PDIR_LEFT;
        desiredVel[0] = MAX(vel._x - maxSpeedHalved, -maxSpeed);
    }
    if (IS_BIT_SET(inpt, GISF_MOVING_DOWN))
    {
        state = STAT_MOVING;
        _stats._dir = PDIR_DOWN;
        desiredVel[1] = MAX(vel._y - maxSpeedHalved, -maxSpeed);
    }
    if (IS_BIT_SET(inpt, GISF_MOVING_RIGHT))
    {
        state = STAT_MOVING;
        _stats._dir = PDIR_RIGHT;
        desiredVel[0] = MIN(vel._x + maxSpeedHalved, maxSpeed);
    }
    
    if (state == STAT_IDLE)
    {
        desiredVel[0] = vel._x * 0.86f;
        desiredVel[1] = vel._y * 0.86f;
    }
    
    _entity->pose()._velocity._x = desiredVel[0];
    _entity->pose()._velocity._y = desiredVel[1];
    sanitizeCloseToZeroVector(_entity->pose()._velocity._x, _entity->pose()._velocity._y, 0.01f);
    
    // I know... but look at the sprite sheet
    _entity->pose()._isFacingLeft = _stats._dir == PDIR_RIGHT;
    
    if (isLocal)
    {
        SoundUtil::playSoundForStateIfChanged(_entity, fromState, state);
    }
}

void PlayerController::setUsername(std::string value)
{
    _playerInfo._username = value;
}

std::string PlayerController::getUsername() const
{
    return _playerInfo._username;
}

void PlayerController::setUserAddress(std::string value)
{
    _playerInfo._userAddress = value;
}

std::string PlayerController::getUserAddress() const
{
    return _playerInfo._userAddress;
}

void PlayerController::setPlayerID(uint8_t value)
{
    _playerInfo._playerID = value;
}

uint8_t PlayerController::getPlayerID() const
{
    return _playerInfo._playerID;
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

IMPL_EntityNetworkController_create(PlayerNetworkController)

void PlayerNetworkController::read(InputMemoryBitStream& imbs)
{
    uint8_t fromState = _entity->stateCache()._state;
    
    EntityNetworkController::read(imbs);
    
    PlayerController* c = static_cast<PlayerController*>(_entity->controller());
    
    bool stateBit;
    
    imbs.read(stateBit);
    if (stateBit)
    {
        imbs.readSmall(c->_playerInfo._username);
        imbs.readSmall(c->_playerInfo._userAddress);
        imbs.read<uint8_t, 3>(c->_playerInfo._playerID);
        
        c->_playerInfoCache = c->_playerInfo;
    }
    
    imbs.read(stateBit);
    if (stateBit)
    {
        imbs.read(c->_stats._health);
        imbs.read(c->_stats._dir);
        
        c->_statsCache = c->_stats;
    }
    
    if (!NW_MGR_CLNT->isPlayerIDLocal(c->_playerInfo._playerID))
    {
        SoundUtil::playSoundForStateIfChanged(_entity, fromState, _entity->state()._state);
    }
}

uint8_t PlayerNetworkController::write(OutputMemoryBitStream& ombs, uint8_t dirtyState)
{
    uint8_t ret = EntityNetworkController::write(ombs, dirtyState);
    
    PlayerController* c = static_cast<PlayerController*>(_entity->controller());
    
    bool RSTF_PLAYER_INFO = IS_BIT_SET(dirtyState, PlayerController::RSTF_PLAYER_INFO);
    ombs.write(RSTF_PLAYER_INFO);
    if (RSTF_PLAYER_INFO)
    {
        ombs.writeSmall(c->_playerInfo._username);
        ombs.writeSmall(c->_playerInfo._userAddress);
        ombs.write<uint8_t, 3>(c->_playerInfo._playerID);
        
        ret |= PlayerController::RSTF_PLAYER_INFO;
    }
    
    bool RSTF_STATS = IS_BIT_SET(dirtyState, PlayerController::RSTF_STATS);
    ombs.write(RSTF_STATS);
    if (RSTF_STATS)
    {
        ombs.write(c->_stats._health);
        ombs.write(c->_stats._dir);
        
        ret |= PlayerController::RSTF_STATS;
    }
    
    return ret;
}

void PlayerNetworkController::recallCache()
{
    EntityNetworkController::recallCache();
    
    PlayerController* c = static_cast<PlayerController*>(_entity->controller());
    
    c->_playerInfo = c->_playerInfoCache;
    c->_stats = c->_statsCache;
}

uint8_t PlayerNetworkController::refreshDirtyState()
{
    uint8_t ret = EntityNetworkController::refreshDirtyState();
    
    PlayerController* c = static_cast<PlayerController*>(_entity->controller());
    
    if (c->_playerInfoCache != c->_playerInfo)
    {
        c->_playerInfoCache = c->_playerInfo;
        ret |= PlayerController::RSTF_PLAYER_INFO;
    }
    
    if (c->_statsCache != c->_stats)
    {
        c->_statsCache = c->_stats;
        ret |= PlayerController::RSTF_STATS;
    }
    
    return ret;
}
