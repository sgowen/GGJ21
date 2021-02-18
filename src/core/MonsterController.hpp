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
    friend class MonsterRenderController;
    
    DECL_RTTI;
    DECL_EntityController_create(EntityController);
    
public:
    MonsterController(Entity* e);
    virtual ~MonsterController() {}
    
    virtual void update() override;
    
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
        
        friend bool operator==(Stats& a, Stats& b)
        {
            return
            a._health == b._health &&
            a._dir == b._dir;
        }
        
        friend bool operator!=(Stats& a, Stats& b)
        {
            return !(a == b);
        }
    };
    Stats _stats;
    Stats _statsCache;
};

#include "EntityNetworkController.hpp"

class MonsterNetworkController : public EntityNetworkController
{
    DECL_EntityController_create(EntityNetworkController);
    
public:
    MonsterNetworkController(Entity* e) : EntityNetworkController(e) {}
    virtual ~MonsterNetworkController() {}
    
    virtual void read(InputMemoryBitStream& imbs);
    virtual uint8_t write(OutputMemoryBitStream& ombs, uint8_t dirtyState);
    virtual void recallCache();
    virtual uint8_t refreshDirtyState();
};

#include "TopDownPhysicsController.hpp"

class MonsterPhysicsController : public TopDownPhysicsController
{
    DECL_RTTI;
    DECL_EntityController_create(EntityPhysicsController);
    
public:
    MonsterPhysicsController(Entity* e) : TopDownPhysicsController(e) {}
    virtual ~MonsterPhysicsController() {}
    
protected:
    virtual void onCollision(Entity* e) override;
};

#include "EntityRenderController.hpp"

class MonsterRenderController : public EntityRenderController
{
    DECL_RTTI;
    DECL_EntityController_create(EntityRenderController);
    
public:
    MonsterRenderController(Entity* e) : EntityRenderController(e) {}
    virtual ~MonsterRenderController() {}
    
    virtual std::string getTextureMapping() override;
    
    void addSpriteForEncounter(SpriteBatcher& sb);

private:
    std::string getTextureMappingForEncounter();
};
