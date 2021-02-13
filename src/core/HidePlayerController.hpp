//
//  HidePlayerController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "PlayerController.hpp"

class HidePlayerController : public PlayerController
{
    friend class HidePlayerNetworkController;
    
    DECL_RTTI;
    DECL_EntityController_create;
    
public:
    HidePlayerController(Entity* e);
    virtual ~HidePlayerController() {}
    
    virtual void update();
    virtual void onMessage(uint16_t message, void* data = NULL);
    virtual std::string getTextureMapping();
    virtual void processInput(InputState* inputState, bool isLocal = false);
    virtual bool isInEncounter();
    virtual uint16_t encounterStateTime();

    std::string getTextureMappingForEncounter();
    float getWidthForEncounter();
    void setEntityLayoutKey(uint32_t value);
    uint32_t getEntityLayoutKey();
    
protected:
    enum ReadStateFlag
    {
        RSTF_ENCOUNTER =          1 << 4,
        RSTF_ENTITY_LAYOUT_INFO = 1 << 5
    };
    
    enum EncounterState
    {
        ESTA_IDLE = 0,
        ESTA_SWING = 1
    };
    
    struct Encounter
    {
        bool _isInCounter;
        uint16_t _stateTime;
        uint8_t _state;
        
        Encounter()
        {
            _isInCounter = false;
            _stateTime = 0;
            _state = ESTA_IDLE;
        }
        
        friend bool operator==(Encounter& a, Encounter& b)
        {
            return
            a._isInCounter == b._isInCounter &&
            a._stateTime == b._stateTime &&
            a._state == b._state;
        }
        
        friend bool operator!=(Encounter& a, Encounter& b)
        {
            return !(a == b);
        }
    };
    Encounter _encounter;
    Encounter _encounterCache;
    
    struct EntityLayoutInfo
    {
        uint32_t _key;
        
        EntityLayoutInfo()
        {
            _key = 0;
        }
        
        friend bool operator==(EntityLayoutInfo& a, EntityLayoutInfo& b)
        {
            return a._key == b._key;
        }
        
        friend bool operator!=(EntityLayoutInfo& a, EntityLayoutInfo& b)
        {
            return !(a == b);
        }
    };
    EntityLayoutInfo _entityLayoutInfo;
    EntityLayoutInfo _entityLayoutInfoCache;
};

class HidePlayerNetworkController : public PlayerNetworkController
{
public:
    DECL_EntityNetworkController_create;
    
    HidePlayerNetworkController(Entity* e, bool isServer) : PlayerNetworkController(e, isServer) {}
    virtual ~HidePlayerNetworkController() {}
    
    virtual void read(InputMemoryBitStream& imbs);
    virtual uint16_t write(OutputMemoryBitStream& ombs, uint16_t dirtyState);
    virtual void recallCache();
    virtual uint16_t refreshDirtyState();
};
