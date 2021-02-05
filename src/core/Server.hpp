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
class TimeTracker;
class EntityIDManager;

class Server
{
public:
    static void create();
    static Server* getInstance();
    static void destroy();
    
    static void sHandleDynamicEntityCreatedOnServer(Entity* e);
    static void sHandleDynamicEntityDeletedOnServer(Entity* e);
    static void sHandleNewClient(uint8_t playerID, std::string playerName);
    static void sHandleLostClient(ClientProxy* inClientProxy, uint8_t index);
    static InputState* sHandleInputStateCreation();
    static void sHandleInputStateRelease(InputState* inputState);
    
    void update();
    uint8_t getPlayerIDForRobotBeingCreated();
    World& getWorld();

private:
    static Server* s_instance;
    
    TimeTracker* _timeTracker;
    EntityIDManager* _entityIDManager;
    World _world;
    Pool<GameInputState> _inputStates;
    uint32_t _map;
    
    void handleNewClient(uint8_t playerID, std::string playerName);
    void handleLostClient(ClientProxy* inClientProxy, uint8_t index);
    InputState* handleInputStateCreation();
    void handleInputStateRelease(InputState* inputState);
    void deletePlayerWithPlayerID(uint8_t playerID);
    void spawnEntityForPlayer(uint8_t playerId, std::string playerName);
    void loadMap();
    void handleDirtyStates(std::vector<Entity*>& entities);
    
    Server();
    ~Server();
    Server(const Server&);
    Server& operator=(const Server&);
};
