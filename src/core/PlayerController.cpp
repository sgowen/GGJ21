//
//  PlayerController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

IMPL_RTTI(PlayerController, EntityController)
IMPL_EntityController_create(PlayerController, EntityController)

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
        _entity->state()._stateTime = 6;
    }
}

void PlayerController::processInput(InputState* is, bool isLive)
{
    uint8_t playerID = _entity->entityDef()._data.getUInt("playerID");
    InputState::PlayerInputState* pis = is->playerInputStateForID(playerID);
    if (pis == NULL)
    {
        return;
    }
    
    uint8_t fromState = _entity->state()._state;
    uint8_t& state = _entity->state()._state;
    uint8_t piss = pis->_inputState;
    
    state = STAT_IDLE;
    Vector2& vel = _entity->velocity();
    if (IS_BIT_SET(piss, GISF_MOVING_UP))
    {
        state = STAT_MOVING;
        _stats._dir = EDIR_UP;
        vel._y = CFG_MAIN.playerMaxTopDownSpeed();
    }
    if (IS_BIT_SET(piss, GISF_MOVING_LEFT))
    {
        state = STAT_MOVING;
        _stats._dir = EDIR_LEFT;
        vel._x = -  CFG_MAIN.playerMaxTopDownSpeed();
    }
    if (IS_BIT_SET(piss, GISF_MOVING_DOWN))
    {
        state = STAT_MOVING;
        _stats._dir = EDIR_DOWN;
        vel._y = -CFG_MAIN.playerMaxTopDownSpeed();
    }
    if (IS_BIT_SET(piss, GISF_MOVING_RIGHT))
    {
        state = STAT_MOVING;
        _stats._dir = EDIR_RIGHT;
        vel._x = CFG_MAIN.playerMaxTopDownSpeed();
    }
    
    if (isLive)
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

uint16_t PlayerController::getHealth()
{
    return _stats._health;
}

IMPL_EntityController_create(PlayerNetworkController, EntityNetworkController)

void PlayerNetworkController::read(InputMemoryBitStream& imbs)
{
    uint8_t fromState = _entity->stateCache()._state;
    
    EntityNetworkController::read(imbs);
    
    PlayerController* c = _entity->controller<PlayerController>();
    
    bool stateBit;
    
    imbs.read(stateBit);
    if (stateBit)
    {
        imbs.readSmall(c->_playerInfo._username);
        imbs.readSmall(c->_playerInfo._userAddress);
        
        c->_playerInfoCache = c->_playerInfo;
    }
    
    imbs.read(stateBit);
    if (stateBit)
    {
        imbs.read(c->_stats._health);
        imbs.read(c->_stats._dir);
        
        c->_statsCache = c->_stats;
    }
    
    uint8_t playerID = _entity->entityDef()._data.getUInt("playerID");
    if (!NW_CLNT->isPlayerIDLocal(playerID))
    {
        SoundUtil::playSoundForStateIfChanged(_entity, fromState, _entity->state()._state);
    }
}

uint8_t PlayerNetworkController::write(OutputMemoryBitStream& ombs, uint8_t dirtyState)
{
    uint8_t ret = EntityNetworkController::write(ombs, dirtyState);
    
    PlayerController* c = _entity->controller<PlayerController>();
    
    bool RSTF_PLAYER_INFO = IS_BIT_SET(dirtyState, PlayerController::RSTF_PLAYER_INFO);
    ombs.write(RSTF_PLAYER_INFO);
    if (RSTF_PLAYER_INFO)
    {
        ombs.writeSmall(c->_playerInfo._username);
        ombs.writeSmall(c->_playerInfo._userAddress);
        
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
    
    PlayerController* c = _entity->controller<PlayerController>();
    
    c->_playerInfo = c->_playerInfoCache;
    c->_stats = c->_statsCache;
}

uint8_t PlayerNetworkController::refreshDirtyState()
{
    uint8_t ret = EntityNetworkController::refreshDirtyState();
    
    PlayerController* c = _entity->controller<PlayerController>();
    
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

IMPL_RTTI(PlayerRenderController, EntityRenderController)
IMPL_EntityController_create(PlayerRenderController, EntityRenderController)

void PlayerRenderController::addSprite(SpriteBatcher& sb)
{
    Entity& e = *_entity;
    TextureRegion& tr = ASSETS.textureRegion(getTextureMapping(), e.stateTime());
    
    PlayerController* ec = e.controller<PlayerController>();
    bool flipX = ec->_stats._dir == EDIR_RIGHT;
    sb.addSprite(tr, e.position()._x, e.position()._y, e.width(), e.height(), e.angle(), flipX);
}
