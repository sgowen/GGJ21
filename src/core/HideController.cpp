//
//  HideController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "HideController.hpp"

#include "MainConfig.hpp"
#include "Macros.hpp"
#include "Entity.hpp"
#include "AudioEngineFactory.hpp"
#include "World.hpp"
#include "MonsterController.hpp"
#include "GameServerEngineState.hpp"
#include "GameClientEngineState.hpp"
#include "EntityRenderController.hpp"
#include "GameInputManager.hpp"

#include <assert.h>

IMPL_RTTI(HideController, PlayerController)
IMPL_EntityController_create(HideController, EntityController)

HideController::HideController(Entity* e) : PlayerController(e),
_encounter(),
_encounterCache(_encounter),
_entityLayoutInfo(),
_entityLayoutInfoCache(_entityLayoutInfo)
{
    // Empty
}

void HideController::update()
{
    PlayerController::update();
    
    if (_encounter._isInCounter)
    {
        ++_encounter._stateTime;
    }
    
    if (_encounter._state == ESTA_SWING)
    {
        if (_encounter._stateTime >= 42)
        {
            _encounter._state = ESTA_IDLE;
            
            World& w = _entity->isServer() ? ENGINE_STATE_GAME_SRVR.getWorld() : ENGINE_STATE_GAME_CLNT.getWorld();
            for (Entity* e : w.getNetworkEntities())
            {
                if (e->controller()->getRTTI().isDerivedFrom(MonsterController::rtti) &&
                    e->controller<MonsterController>()->isInEncounter())
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

void HideController::onMessage(uint16_t message)
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

void HideController::processInput(InputState* is, bool isLive)
{
    InputState::PlayerInputState* pis = is->playerInputStateForID(getPlayerID());
    if (pis == NULL)
    {
        return;
    }
    
    if (_encounter._isInCounter)
    {
        uint8_t piss = pis->_inputState;
        if (_encounter._state == ESTA_IDLE)
        {
            if (IS_BIT_SET(piss, GISF_CONFIRM))
            {
                _encounter._state = ESTA_SWING;
                _encounter._stateTime = 0;
                
                if (isLive)
                {
                    AUDIO_ENGINE.playSound(_entity->renderController()->getSoundMapping(4));
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
        PlayerController::processInput(is, isLive);
    }
}

bool HideController::isInEncounter()
{
    return _encounter._isInCounter;
}

void HideController::setEntityLayoutKey(uint32_t value)
{
    _entityLayoutInfo._key = value;
}

uint32_t HideController::getEntityLayoutKey()
{
    return _entityLayoutInfo._key;
}

#include "InputMemoryBitStream.hpp"
#include "OutputMemoryBitStream.hpp"

IMPL_EntityController_create(HideNetworkController, EntityNetworkController)

void HideNetworkController::read(InputMemoryBitStream& imbs)
{
    PlayerNetworkController::read(imbs);
    
    HideController* ec = _entity->controller<HideController>();
    
    bool stateBit;
    
    imbs.read(stateBit);
    if (stateBit)
    {
        imbs.read(ec->_encounter._isInCounter);
        imbs.read(ec->_encounter._state);
        imbs.read(ec->_encounter._stateTime);
        
        if (!ec->_encounterCache._isInCounter && ec->_encounter._isInCounter)
        {
            AUDIO_ENGINE.playSound(_entity->renderController()->getSoundMapping(2));
        }
        else if (ec->_encounterCache._isInCounter && !ec->_encounter._isInCounter)
        {
            AUDIO_ENGINE.playSound(_entity->renderController()->getSoundMapping(3));
        }
        
        ec->_encounterCache = ec->_encounter;
    }
    
    imbs.read(stateBit);
    if (stateBit)
    {
        imbs.read(ec->_entityLayoutInfo._key);
        
        ec->_entityLayoutInfoCache = ec->_entityLayoutInfo;
    }
}

uint8_t HideNetworkController::write(OutputMemoryBitStream& ombs, uint8_t dirtyState)
{
    uint8_t ret = PlayerNetworkController::write(ombs, dirtyState);
    
    HideController* ec = _entity->controller<HideController>();
    
    bool RSTF_ENCOUNTER = IS_BIT_SET(dirtyState, HideController::RSTF_ENCOUNTER);
    ombs.write(RSTF_ENCOUNTER);
    if (RSTF_ENCOUNTER)
    {
        ombs.write(ec->_encounter._isInCounter);
        ombs.write(ec->_encounter._state);
        ombs.write(ec->_encounter._stateTime);
        
        ret |= HideController::RSTF_ENCOUNTER;
    }
    
    bool RSTF_ENTITY_LAYOUT_INFO = IS_BIT_SET(dirtyState, HideController::RSTF_ENTITY_LAYOUT_INFO);
    ombs.write(RSTF_ENTITY_LAYOUT_INFO);
    if (RSTF_ENTITY_LAYOUT_INFO)
    {
        ombs.write(ec->_entityLayoutInfo._key);
        
        ret |= HideController::RSTF_ENTITY_LAYOUT_INFO;
    }
    
    return ret;
}

void HideNetworkController::recallCache()
{
    PlayerNetworkController::recallCache();
    
    HideController* ec = _entity->controller<HideController>();
    
    ec->_encounter = ec->_encounterCache;
    ec->_entityLayoutInfo = ec->_entityLayoutInfoCache;
}

uint8_t HideNetworkController::refreshDirtyState()
{
    uint8_t ret = PlayerNetworkController::refreshDirtyState();
    
    HideController* ec = _entity->controller<HideController>();
    
    if (ec->_encounterCache != ec->_encounter)
    {
        ec->_encounterCache = ec->_encounter;
        ret |= HideController::RSTF_ENCOUNTER;
    }
    
    if (ec->_entityLayoutInfoCache != ec->_entityLayoutInfo)
    {
        ec->_entityLayoutInfoCache = ec->_entityLayoutInfo;
        ret |= HideController::RSTF_ENTITY_LAYOUT_INFO;
    }
    
    return ret;
}

#include "AssetsManager.hpp"
#include "SpriteBatcher.hpp"

IMPL_RTTI(HideRenderController, EntityRenderController)
IMPL_EntityController_create(HideRenderController, EntityRenderController)

std::string HideRenderController::getTextureMapping()
{
    HideController* ec = _entity->controller<HideController>();
    
    switch (ec->_stats._dir)
    {
        case EDIR_UP:
            return "HIDE_UP";
        case EDIR_LEFT:
        case EDIR_RIGHT:
            return "HIDE_LEFT";
        case EDIR_DOWN:
            return "HIDE_DOWN";
    }
    
    return EntityRenderController::getTextureMapping();
}

void HideRenderController::addSpriteForEncounter(SpriteBatcher& sb)
{
    HideController* ec = _entity->controller<HideController>();
    TextureRegion tr = ASSETS.textureRegion(getTextureMappingForEncounter(), ec->_encounter._stateTime);
    sb.addSprite(tr, CFG_MAIN.playerBattleX(), CFG_MAIN.playerBattleY(), getWidthForEncounter(), CFG_MAIN.playerBattleHeight(), 0);
}

std::string HideRenderController::getTextureMappingForEncounter()
{
    HideController* ec = _entity->controller<HideController>();
    assert(ec->_encounter._state <= HideController::ESTA_SWING);
    
    switch (ec->_encounter._state)
    {
        case HideController::ESTA_IDLE:
            return "BIG_HIDE_IDLE";
        case HideController::ESTA_SWING:
        default:
            return ec->_encounter._stateTime >= 35 ? "BIG_HIDE_SWING_3" : ec->_encounter._stateTime >= 14 ? "BIG_HIDE_SWING_2" : "BIG_HIDE_SWING_1";
    }
}

float HideRenderController::getWidthForEncounter()
{
    HideController* ec = _entity->controller<HideController>();
    assert(ec->_encounter._state <= HideController::ESTA_SWING);
    
    switch (ec->_encounter._state)
    {
        case HideController::ESTA_IDLE:
            return CFG_MAIN.playerBattleWidth();
        case HideController::ESTA_SWING:
        default:
            return ec->_encounter._stateTime >= 35 ? CFG_MAIN.playerBattleWidth() : ec->_encounter._stateTime >= 14 ? (CFG_MAIN.playerBattleWidth() + 4) : CFG_MAIN.playerBattleWidth();
    }
}
