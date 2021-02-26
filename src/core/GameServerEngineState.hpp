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
#include "InputState.hpp"
#include "World.hpp"

#include <string>
#include <vector>

class Engine;
class ClientProxy;
class Entity;

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
    void resetWorld();
    void populateFromEntityLayout(EntityLayoutDef& eld);
    void restart();
    World& getWorld();
    
private:
    World _world;
    bool _isRestarting;
    
    void update(Engine* e);
    void updateWorld(int moveIndex);
    void handleDirtyStates(std::vector<Entity*>& entities);
    void addPlayer(std::string username, uint8_t playerID);
    void removePlayer(uint8_t playerID);
    
    GameServerEngineState();
    virtual ~GameServerEngineState() {}
    GameServerEngineState(const GameServerEngineState&);
    GameServerEngineState& operator=(const GameServerEngineState&);
};
