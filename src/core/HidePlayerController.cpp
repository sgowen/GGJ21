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
    PlayerController::update();
    
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

void HidePlayerController::processInput(InputState* inputState, bool isLive)
{
    GameInputState* is = static_cast<GameInputState*>(inputState);
    GameInputState::PlayerInputState* pis = is->getPlayerInputStateForID(getPlayerID());
    if (pis == NULL)
    {
        return;
    }
    
    if (_encounter._isInCounter)
    {
        uint8_t piss = pis->inputState();
        if (_encounter._state == ESTA_IDLE)
        {
            if (IS_BIT_SET(piss, GISF_CONFIRM))
            {
                _encounter._state = ESTA_SWING;
                _encounter._stateTime = 0;
                
                if (isLive)
                {
                    GOW_AUDIO.playSound(_entity->getSoundMapping(4));
                }
            }
        }
        else
        {
            if (IS_BIT_SET(piss, GISF_CANCEL))
            {
                _encounter._state = ESTA_IDLE;
                _encounter._stateTime = 0;
            }
        }
    }
    else
    {
        PlayerController::processInput(inputState, isLive);
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
        imbs.read(c->_encounter._state);
        imbs.read(c->_encounter._stateTime);
        
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

uint8_t HidePlayerNetworkController::write(OutputMemoryBitStream& ombs, uint8_t dirtyState)
{
    uint8_t ret = PlayerNetworkController::write(ombs, dirtyState);
    
    HidePlayerController* c = static_cast<HidePlayerController*>(_entity->controller());
    
    bool RSTF_ENCOUNTER = IS_BIT_SET(dirtyState, HidePlayerController::RSTF_ENCOUNTER);
    ombs.write(RSTF_ENCOUNTER);
    if (RSTF_ENCOUNTER)
    {
        ombs.write(c->_encounter._isInCounter);
        ombs.write(c->_encounter._state);
        ombs.write(c->_encounter._stateTime);
        
        ret |= HidePlayerController::RSTF_ENCOUNTER;
    }
    
    bool RSTF_ENTITY_LAYOUT_INFO = IS_BIT_SET(dirtyState, HidePlayerController::RSTF_ENTITY_LAYOUT_INFO);
    ombs.write(RSTF_ENTITY_LAYOUT_INFO);
    if (RSTF_ENTITY_LAYOUT_INFO)
    {
        ombs.write(c->_entityLayoutInfo._key);
        
        ret |= HidePlayerController::RSTF_ENTITY_LAYOUT_INFO;
    }
    
    return ret;
}

void HidePlayerNetworkController::recallCache()
{
    PlayerNetworkController::recallCache();
    
    HidePlayerController* c = static_cast<HidePlayerController*>(_entity->controller());
    
    c->_encounter = c->_encounterCache;
    c->_entityLayoutInfo = c->_entityLayoutInfoCache;
}

uint8_t HidePlayerNetworkController::refreshDirtyState()
{
    uint8_t ret = PlayerNetworkController::refreshDirtyState();
    
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
