//
//  Server.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "Pool.hpp"
#include "GameInputState.hpp"
#include "World.hpp"

#include <string>
#include <vector>

class ClientProxy;
class Entity;
struct MapDef;

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

class Server
{
public:
    static void create();
    static Server* getInstance();
    static void destroy();
    
    void handleNewClient(std::string username, uint8_t playerID);
    void handleLostClient(ClientProxy& cp, uint8_t index);
    InputState* handleInputStateCreation();
    void handleInputStateRelease(InputState* inputState);
    void loadEntityLayout(EntityLayoutDef& eld);
    void restart();
    std::vector<PlayerDef>& getPlayers();
    void update();
    World& getWorld();

private:
    static Server* s_instance;
    
    World _world;
    Pool<GameInputState> _poolGameInputState;
    bool _isRestarting;
    std::vector<PlayerDef> _players;
    
    void registerPlayer(std::string username, uint8_t playerID);
    void removePlayer(uint8_t playerID);
    void handleDirtyStates(std::vector<Entity*>& entities);
    void removeProcessedMoves();
    
    Server();
    ~Server();
    Server(const Server&);
    Server& operator=(const Server&);
};
