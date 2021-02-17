//
//  GameServerEngineState.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "StateMachine.hpp"

#include "Pool.hpp"
#include "GameInputState.hpp"
#include "World.hpp"

#include <string>
#include <vector>

class Engine;
class ClientProxy;
class Entity;

struct PlayerDef
{
    std::string _username;
    uint8_t _playerID;
    
    PlayerDef(std::string username, uint8_t playerID) :
    _username(username),
    _playerID(playerID)
    {
        // Empty
    }
};

#define ENGINE_STATE_GAME_SRVR GameServerEngineState::getInstance()

class GameServerEngineState : public State<Engine>
{
public:
    static GameServerEngineState& getInstance()
    {
        static GameServerEngineState ret = GameServerEngineState();
        return ret;
    }
    
    virtual void enter(Engine* e);
    virtual void execute(Engine* e);
    virtual void exit(Engine* e);
    
    void handleNewClient(std::string username, uint8_t playerID);
    void handleLostClient(ClientProxy& cp, uint8_t localPlayerIndex);
    InputState* handleInputStateCreation();
    void handleInputStateRelease(InputState* inputState);
    void resetWorld();
    void loadEntityLayout(EntityLayoutDef& eld);
    void restart();
    std::vector<PlayerDef>& getPlayers();
    World& getWorld();
    
private:
    World _world;
    Pool<GameInputState> _poolGameInputState;
    bool _isRestarting;
    std::vector<PlayerDef> _players;
    
    void update(Engine* e);
    void updateWorld(int moveIndex);
    void registerPlayer(std::string username, uint8_t playerID);
    void removePlayer(uint8_t playerID);
    void handleDirtyStates(std::vector<Entity*>& entities);
    void removeProcessedMoves();
    
    GameServerEngineState();
    ~GameServerEngineState() {}
    GameServerEngineState(const GameServerEngineState&);
    GameServerEngineState& operator=(const GameServerEngineState&);
};
