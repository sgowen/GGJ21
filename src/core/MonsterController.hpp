//
//  MonsterController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "EntityController.hpp"

enum MonsterDirection
{
    MDIR_UP      = 0,
    MDIR_DOWN    = 1
};

class MonsterController : public EntityController
{
    friend class MonsterNetworkController;
    
    DECL_RTTI;
    DECL_EntityController_create;
    
public:
    MonsterController(Entity* e);
    virtual ~MonsterController() {}
    
    virtual void update();
    virtual void receiveMessage(uint16_t message, void* data = NULL);
    virtual std::string getTextureMapping(uint8_t state);
    virtual void onCollision(Entity* e);
    
    std::string getTextureMappingForEncounter();
    
private:
    enum State
    {
        STAT_IDLE   = 0,
        STAT_MOVING = 1
    };
    
    enum ReadStateFlag
    {
        RSTF_STATS = 1 << 2
    };
    
    struct Stats
    {
        uint16_t _health;
        uint8_t _dir;
        
        Stats()
        {
            _health = 3;
            _dir = MDIR_DOWN;
        }
        
        friend bool operator==(Stats& lhs, Stats& rhs)
        {
            return
            lhs._health == rhs._health &&
            lhs._dir == rhs._dir;
        }
        
        friend bool operator!=(Stats& lhs, Stats& rhs)
        {
            return !(lhs == rhs);
        }
    };
    Stats _stats;
    Stats _statsNetworkCache;
};

#include "EntityNetworkController.hpp"

class MonsterNetworkController : public EntityNetworkController
{
    DECL_EntityNetworkController_create;
    
public:
    MonsterNetworkController(Entity* e, bool isServer);
    virtual ~MonsterNetworkController() {}
    
    virtual void read(InputMemoryBitStream& ip);
    virtual uint16_t write(OutputMemoryBitStream& op, uint16_t dirtyState);
    
    virtual void recallNetworkCache();
    virtual uint16_t getDirtyState();
    
private:
    MonsterController* _controller;
};
