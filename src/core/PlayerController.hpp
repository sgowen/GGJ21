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
    
    DECL_RTTI;
    DECL_EntityController_create;
    
public:
    PlayerController(Entity* e);
    virtual ~PlayerController() {}
    
    virtual void update();
    virtual void receiveMessage(uint16_t message, void* data = NULL);
    virtual void onFixturesCreated(std::vector<b2Fixture*>& fixtures) {}
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB) { return false; }
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB) {}
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB) {}
    
    void processInput(InputState* inputState);
    void enforceBounds(Rektangle& bounds);
    
    void setAddressHash(uint64_t inValue);
    uint64_t getAddressHash() const;
    void setPlayerID(uint8_t inValue);
    uint8_t getPlayerID() const;
    void setMap(uint32_t inValue);
    uint32_t getMap() const;
    void setPlayerName(std::string inValue);
    std::string& getPlayerName();
    
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
        RSTF_PLAYER_INFO = 1 << 2,
        RSTF_STATS =       1 << 3
    };
    
    struct PlayerInfo
    {
        uint64_t _addressHash;
        uint8_t _playerID;
        std::string _playerName;
        
        PlayerInfo()
        {
            _addressHash = 0;
            _playerID = 0;
            _playerName = "Unknown";
        }
        
        friend bool operator==(PlayerInfo& lhs, PlayerInfo& rhs)
        {
            return
            lhs._addressHash == rhs._addressHash &&
            lhs._playerID    == rhs._playerID &&
            lhs._playerName  == rhs._playerName;
        }
        
        friend bool operator!=(PlayerInfo& lhs, PlayerInfo& rhs)
        {
            return !(lhs == rhs);
        }
    };
    PlayerInfo _playerInfo;
    PlayerInfo _playerInfoNetworkCache;
    
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

class PlayerNetworkController : public EntityNetworkController
{
    DECL_EntityNetworkController_create;
    
public:
    PlayerNetworkController(Entity* e, bool isServer);
    virtual ~PlayerNetworkController() {}
    
    virtual void read(InputMemoryBitStream& ip);
    virtual uint16_t write(OutputMemoryBitStream& op, uint16_t dirtyState);
    
    virtual void recallNetworkCache();
    virtual uint16_t getDirtyState();
    
    bool isLocalPlayer();
    
private:
    PlayerController* _controller;
    bool _isLocalPlayer;
};
