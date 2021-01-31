//
//  MonsterController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "EntityController.hpp"

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
    virtual void onFixturesCreated(std::vector<b2Fixture*>& fixtures) {}
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB) { return false; }
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB) {}
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB) {}
    
private:
    enum State
    {
        STAT_IDLE_UP      = 0,
        STAT_IDLE_DOWN    = 1,
        STAT_IDLE_LEFT    = 2,
        STAT_IDLE_RIGHT   = 3,
        STAT_MOVING_UP    = 4,
        STAT_MOVING_DOWN  = 5,
        STAT_MOVING_LEFT  = 6,
        STAT_MOVING_RIGHT = 7
    };
    
    enum ReadStateFlag
    {
        RSTF_STATS = 1 << 2
    };
    
    struct Stats
    {
        uint16_t _health;
        
        Stats()
        {
            _health = 3;
        }
        
        friend bool operator==(Stats& lhs, Stats& rhs)
        {
            return
            lhs._health == rhs._health;
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
