//
//  PlayerController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "EntityController.hpp"

#include <string>

class InputState;
struct Rektangle;

class PlayerController : public EntityController
{
    friend class PlayerNetworkController;
    friend class PlayerRenderController;
    
    DECL_RTTI;
    DECL_EntityController_create(EntityController);
    
public:
    PlayerController(Entity* e);
    virtual ~PlayerController() {}
    
    virtual void update() override;
    virtual void processInput(InputState* is, bool isLive);
    
    void setUsername(std::string value);
    std::string getUsername() const;
    void setUserAddress(std::string value);
    std::string getUserAddress() const;
    uint16_t getHealth();
    
protected:
    enum State
    {
        STAT_IDLE = 0,
        STAT_MOVING = 1
    };
    
    enum ReadStateFlag
    {
        RSTF_PLAYER_INFO = 1 << 2,
        RSTF_STATS =       1 << 3
    };
    
    struct PlayerInfo
    {
        std::string _username;
        std::string _userAddress;
        
        PlayerInfo()
        {
            _username = "Unknown";
            _userAddress = "1.3.3.7";
        }
        
        friend bool operator==(PlayerInfo& a, PlayerInfo& b)
        {
            return
            a._username    == b._username &&
            a._userAddress == b._userAddress;
        }
        
        friend bool operator!=(PlayerInfo& a, PlayerInfo& b)
        {
            return !(a == b);
        }
    };
    PlayerInfo _playerInfo;
    PlayerInfo _playerInfoCache;
    
    struct Stats
    {
        uint16_t _health;
        uint8_t _dir;
        
        Stats()
        {
            _health = 3;
            _dir = 0;
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

class PlayerNetworkController : public EntityNetworkController
{
public:
    DECL_EntityController_create(EntityNetworkController);
    
    PlayerNetworkController(Entity* e) : EntityNetworkController(e) {}
    virtual ~PlayerNetworkController() {}
    
    virtual void read(InputMemoryBitStream& imbs);
    virtual uint8_t write(OutputMemoryBitStream& ombs, uint8_t dirtyState);
    virtual void recallCache();
    virtual uint8_t refreshDirtyState();
};

#include "EntityRenderController.hpp"

class PlayerRenderController : public EntityRenderController
{
    DECL_RTTI;
    DECL_EntityController_create(EntityRenderController);
    
public:
    PlayerRenderController(Entity* e) : EntityRenderController(e) {}
    virtual ~PlayerRenderController() {}
    
    virtual void addSprite(SpriteBatcher& sb) override;
};
