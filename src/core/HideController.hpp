//
//  HideController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "PlayerController.hpp"

class HideController : public PlayerController
{
    friend class HideNetworkController;
    friend class HideRenderController;
    
    DECL_RTTI;
    DECL_EntityController_create(EntityController);
    
public:
    HideController(Entity* e);
    virtual ~HideController() {}
    
    virtual void update() override;
    virtual void onMessage(uint16_t message, void* data = NULL) override;
    virtual void processInput(InputState* inputState, bool isLive) override;
    bool isInEncounter();
    uint16_t encounterStateTime();
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
        uint8_t _state;
        uint16_t _stateTime;
        
        Encounter()
        {
            _isInCounter = false;
            _state = ESTA_IDLE;
            _stateTime = 0;
        }
        
        friend bool operator==(Encounter& a, Encounter& b)
        {
            return
            a._isInCounter == b._isInCounter &&
            a._state == b._state &&
            a._stateTime == b._stateTime;
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

class HideNetworkController : public PlayerNetworkController
{
public:
    DECL_EntityController_create(EntityNetworkController);
    
    HideNetworkController(Entity* e) : PlayerNetworkController(e) {}
    virtual ~HideNetworkController() {}
    
    virtual void read(InputMemoryBitStream& imbs);
    virtual uint8_t write(OutputMemoryBitStream& ombs, uint8_t dirtyState);
    virtual void recallCache();
    virtual uint8_t refreshDirtyState();
};

class HideRenderController : public PlayerRenderController
{
    DECL_RTTI;
    DECL_EntityController_create(EntityRenderController);
    
public:
    HideRenderController(Entity* e) : PlayerRenderController(e) {}
    virtual ~HideRenderController() {}
    
    virtual std::string getTextureMapping() override;
    
    void addSpriteForEncounter(SpriteBatcher& sb);

private:
    std::string getTextureMappingForEncounter();
    float getWidthForEncounter();
};
