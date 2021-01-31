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

enum PlayerDirection
{
    PDIR_UP      = 0,
    PDIR_DOWN    = 1,
    PDIR_LEFT    = 2,
    PDIR_RIGHT   = 3
};

class PlayerController : public EntityController
{
    friend class PlayerNetworkController;
    
    DECL_RTTI;
    DECL_EntityController_create;
    
public:
    PlayerController(Entity* e);
    virtual ~PlayerController() {}
        
    virtual void update();
    virtual void onMessage(uint16_t message, void* data = NULL);
    
    void processInput(InputState* inputState);
    void enforceBounds(Rektangle& bounds);
    bool isInEncounter();
    uint16_t encounterStateTime();
    
    void setAddressHash(uint64_t inValue);
    uint64_t getAddressHash() const;
    void setPlayerID(uint8_t inValue);
    uint8_t getPlayerID() const;
    void setMap(uint32_t inValue);
    uint32_t getMap() const;
    void setPlayerName(std::string inValue);
    std::string& getPlayerName();
    PlayerDirection getPlayerDirection();
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
        RSTF_STATS =       1 << 3,
        RSTF_ENCOUNTER =   1 << 4
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
        uint8_t _dir;
        
        Stats()
        {
            _health = 3;
            _dir = PDIR_UP;
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
        
        friend bool operator==(Encounter& lhs, Encounter& rhs)
        {
            return
            lhs._isInCounter == rhs._isInCounter &&
            lhs._stateTime == rhs._stateTime &&
            lhs._state == rhs._state;
        }
        
        friend bool operator!=(Encounter& lhs, Encounter& rhs)
        {
            return !(lhs == rhs);
        }
    };
    Encounter _encounter;
    Encounter _encounterNetworkCache;
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
