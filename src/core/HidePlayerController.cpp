//
//  HidePlayerController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "HidePlayerController.hpp"

#include "MainConfig.hpp"
#include "Macros.hpp"
#include "Entity.hpp"
#include "GowAudioEngine.hpp"
#include "World.hpp"
#include "MonsterController.hpp"
#include "Server.hpp"

#include <assert.h>

IMPL_RTTI(HidePlayerController, PlayerController)
IMPL_EntityController_create(HidePlayerController)

HidePlayerController::HidePlayerController(Entity* e) : PlayerController(e),
_encounter(),
_encounterCache(_encounter),
_entityLayoutInfo(),
_entityLayoutInfoCache(_entityLayoutInfo)
{
    // Empty
}

void HidePlayerController::update()
{
    if (_encounter._isInCounter)
    {
        ++_encounter._stateTime;
    }
    
    if (!_entity->networkController()->isServer())
    {
        return;
    }
    
    if (_encounter._state == ESTA_SWING)
    {
        if (_encounter._stateTime >= 42)
        {
            _encounter._state = ESTA_IDLE;
            
            World& w = Server::getInstance()->getWorld();
            for (Entity* e : w.getNetworkEntities())
            {
                if (e->controller()->getRTTI().isDerivedFrom(MonsterController::rtti))
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

void HidePlayerController::onMessage(uint16_t message, void* data)
{
    switch (message)
    {
        case MSG_ENCOUNTER:
        {
            if (!_encounter._isInCounter)
            {
                _encounter._isInCounter = true;
                _encounter._stateTime = 0;
                _entity->state()._state = STAT_IDLE;
                _entity->pose()._velocity._x = 0;
                _entity->pose()._velocity._y = 0;
            }
            break;
        }
        default:
            break;
    }
}

std::string HidePlayerController::getTextureMapping()
{
    switch (_stats._dir)
    {
        case PDIR_UP:
            return "HIDE_UP";
        case PDIR_LEFT:
        case PDIR_RIGHT:
            return "HIDE_LEFT";
        case PDIR_DOWN:
            return "HIDE_DOWN";
        default:
            assert(false);
    }
}

void HidePlayerController::processInput(InputState* inputState, bool isLocal)
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
            uint8_t inpt = playerInputState->inputState();
            if (IS_BIT_SET(inpt, GISF_MOVING_RIGHT))
            {
                _encounter._state = ESTA_SWING;
                _encounter._stateTime = 0;
                
                if (isLocal)
                {
                    GOW_AUDIO.playSound(_entity->getSoundMapping(4));
                }
            }
        }
    }
    else
    {
        PlayerController::processInput(inputState, isLocal);
    }
}

bool HidePlayerController::isInEncounter()
{
    return _encounter._isInCounter;
}

uint16_t HidePlayerController::encounterStateTime()
{
    return _encounter._stateTime;
}

std::string HidePlayerController::getTextureMappingForEncounter()
{
    switch (_encounter._state)
    {
        case ESTA_IDLE:
            return "BIG_HIDE_IDLE";
        case ESTA_SWING:
            return _encounter._stateTime >= 35 ? "BIG_HIDE_SWING_3" : _encounter._stateTime >= 14 ? "BIG_HIDE_SWING_2" : "BIG_HIDE_SWING_1";
        default:
            assert(false);
    }
}

float HidePlayerController::getWidthForEncounter()
{
    switch (_encounter._state)
    {
        case ESTA_IDLE:
            return CFG_MAIN._playerBattleWidth;
        case ESTA_SWING:
            return _encounter._stateTime >= 35 ? CFG_MAIN._playerBattleWidth : _encounter._stateTime >= 14 ? (CFG_MAIN._playerBattleWidth + 4) : CFG_MAIN._playerBattleWidth;
        default:
            assert(false);
    }
}

void HidePlayerController::setEntityLayoutKey(uint32_t value)
{
    _entityLayoutInfo._key = value;
}

uint32_t HidePlayerController::getEntityLayoutKey()
{
    return _entityLayoutInfo._key;
}

#include "InputMemoryBitStream.hpp"
#include "OutputMemoryBitStream.hpp"

IMPL_EntityNetworkController_create(HidePlayerNetworkController)

void HidePlayerNetworkController::read(InputMemoryBitStream& imbs)
{
    PlayerNetworkController::read(imbs);
    
    HidePlayerController* c = static_cast<HidePlayerController*>(_entity->controller());
    
    bool stateBit;
    
    imbs.read(stateBit);
    if (stateBit)
    {
        imbs.read(c->_encounter._isInCounter);
        imbs.read(c->_encounter._stateTime);
        imbs.read(c->_encounter._state);
        
        if (!c->_encounterCache._isInCounter && c->_encounter._isInCounter)
        {
            GOW_AUDIO.playSound(_entity->getSoundMapping(2));
        }
        else if (c->_encounterCache._isInCounter && !c->_encounter._isInCounter)
        {
            GOW_AUDIO.playSound(_entity->getSoundMapping(3));
        }
        
        c->_encounterCache = c->_encounter;
    }
    
    imbs.read(stateBit);
    if (stateBit)
    {
        imbs.read(c->_entityLayoutInfo._key);
        
        c->_entityLayoutInfoCache = c->_entityLayoutInfo;
    }
}

uint16_t HidePlayerNetworkController::write(OutputMemoryBitStream& ombs, uint16_t dirtyState)
{
    uint16_t writtenState = PlayerNetworkController::write(ombs, dirtyState);
    
    HidePlayerController* c = static_cast<HidePlayerController*>(_entity->controller());
    
    bool RSTF_ENCOUNTER = IS_BIT_SET(dirtyState, HidePlayerController::RSTF_ENCOUNTER);
    ombs.write(RSTF_ENCOUNTER);
    if (RSTF_ENCOUNTER)
    {
        ombs.write(c->_encounter._isInCounter);
        ombs.write(c->_encounter._stateTime);
        ombs.write(c->_encounter._state);
        
        writtenState |= HidePlayerController::RSTF_ENCOUNTER;
    }
    
    bool RSTF_ENTITY_LAYOUT_INFO = IS_BIT_SET(dirtyState, HidePlayerController::RSTF_ENTITY_LAYOUT_INFO);
    ombs.write(RSTF_ENTITY_LAYOUT_INFO);
    if (RSTF_ENTITY_LAYOUT_INFO)
    {
        ombs.write(c->_entityLayoutInfo._key);
        
        writtenState |= HidePlayerController::RSTF_ENTITY_LAYOUT_INFO;
    }
    
    return writtenState;
}

void HidePlayerNetworkController::recallCache()
{
    PlayerNetworkController::recallCache();
    
    HidePlayerController* c = static_cast<HidePlayerController*>(_entity->controller());
    
    c->_encounter = c->_encounterCache;
    c->_entityLayoutInfo = c->_entityLayoutInfoCache;
}

uint16_t HidePlayerNetworkController::refreshDirtyState()
{
    uint16_t ret = PlayerNetworkController::refreshDirtyState();
    
    HidePlayerController* c = static_cast<HidePlayerController*>(_entity->controller());
    
    if (c->_encounterCache != c->_encounter)
    {
        c->_encounterCache = c->_encounter;
        ret |= HidePlayerController::RSTF_ENCOUNTER;
    }
    
    if (c->_entityLayoutInfoCache != c->_entityLayoutInfo)
    {
        c->_entityLayoutInfoCache = c->_entityLayoutInfo;
        ret |= HidePlayerController::RSTF_ENTITY_LAYOUT_INFO;
    }
    
    return ret;
}
