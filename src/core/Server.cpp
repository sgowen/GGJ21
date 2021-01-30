//
//  Server.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "Server.hpp"

#include "ClientProxy.hpp"
#include "Entity.hpp"
#include "Timing.hpp"
#include "EntityIDManager.hpp"

#include "NetworkManagerServer.hpp"
#include "EntityManager.hpp"
#include "SocketServerHelper.hpp"
#include "MachineAddress.hpp"
#include "Constants.hpp"
#include "PlayerController.hpp"
#include "EntityMapper.hpp"
#include "EntityLayoutMapper.hpp"
#include "InstanceManager.hpp"
#include "MainConfig.hpp"
#include "Network.hpp"

#include <ctime>
#include <assert.h>

#define SERVER_CALLBACKS Server::sHandleNewClient, Server::sHandleLostClient, Server::sHandleInputStateCreation, Server::sHandleInputStateRelease

Server* Server::s_instance = NULL;

void Server::create()
{
    assert(!s_instance);
    
    s_instance = new Server();
}

Server * Server::getInstance()
{
    return s_instance;
}

void Server::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = NULL;
}

void Server::sHandleNewClient(uint8_t playerID, std::string playerName)
{
    getInstance()->handleNewClient(playerID, playerName);
}

void Server::sHandleLostClient(ClientProxy* cp, uint8_t index)
{
    getInstance()->handleLostClient(cp, index);
}

InputState* Server::sHandleInputStateCreation()
{
    return getInstance()->handleInputStateCreation();
}

void Server::sHandleInputStateRelease(InputState* inputState)
{
    getInstance()->handleInputStateRelease(inputState);
}

void Server::update()
{
    _timing->onFrame();
    
    NW_MGR_SERVER->processIncomingPackets();
    
    int moveCount = NW_MGR_SERVER->getMoveCount();
    if (moveCount > 0)
    {
        for (int i = 0; i < moveCount; ++i)
        {
            for (Entity* e : _world.getPlayers())
            {
                PlayerController* pc = static_cast<PlayerController*>(e->getController());
                
                ClientProxy* client = NW_MGR_SERVER->getClientProxy(pc->getPlayerID());
                if (client)
                {
                    MoveList& moveList = client->getUnprocessedMoveList();
                    Move* move = moveList.getMoveAtIndex(i);
                    if (move)
                    {
                        pc->processInput(move->getInputState());
                        
                        moveList.markMoveAsProcessed(move);
                        
                        client->setLastMoveTimestampDirty(true);
                    }
                }
            }
            
            _world.stepPhysics();
            
            std::vector<Entity*> toDelete;
            
            for (Entity* e : _world.getDynamicEntities())
            {
                if (!e->isRequestingDeletion())
                {
                    e->update();
                }
                
                if (e->isRequestingDeletion())
                {
                    toDelete.push_back(e);
                }
            }
            
            for (Entity* e : toDelete)
            {
                NW_MGR_SERVER->deregisterEntity(e);
                
                EntityController* c = e->getController();
                if (c->getRTTI().derivesFrom(PlayerController::rtti))
                {
                    PlayerController* pc = static_cast<PlayerController*>(c);
                    assert(pc);
                    
                    // This is my shoddy respawn implementation
                    getInstance()->spawnEntityForPlayer(pc->getPlayerID(), pc->getPlayerName());
                }
                
                _world.removeEntity(e);
            }
            
            handleDirtyStates(_world.getDynamicEntities());
        }
        
        for (uint8_t i = 0; i < CFG_MAIN._maxNumPlayers; ++i)
        {
            uint8_t playerID = i + 1;
            ClientProxy* client = NW_MGR_SERVER->getClientProxy(playerID);
            if (client)
            {
                MoveList& moveList = client->getUnprocessedMoveList();
                moveList.removeProcessedMoves(client->getUnprocessedMoveList().getLastProcessedMoveTimestamp(), Server::sHandleInputStateRelease);
            }
        }
    }
    
    NW_MGR_SERVER->sendOutgoingPackets();
}

World& Server::getWorld()
{
    return _world;
}

void Server::handleNewClient(uint8_t playerID, std::string playerName)
{
    if (NW_MGR_SERVER->getNumClientsConnected() == 1)
    {
        // Time to play!
        
        _map = 'R001';
        loadMap();
    }
    
    spawnEntityForPlayer(playerID, playerName);
}

void Server::handleLostClient(ClientProxy* cp, uint8_t index)
{
    if (index >= 1)
    {
        uint8_t playerID = cp->getPlayerID(index);
        
        deleteRobotWithPlayerId(playerID);
    }
    else
    {
        for (int i = 0; i < cp->getNumPlayers(); ++i)
        {
            uint8_t playerID = cp->getPlayerID(i);
            
            deleteRobotWithPlayerId(playerID);
        }
    }
}

InputState* Server::handleInputStateCreation()
{
    InputState* ret = _inputStates.obtain();
    ret->reset();
    
    return ret;
}

void Server::handleInputStateRelease(InputState* inputState)
{
    GameInputState* gameInputState = static_cast<GameInputState*>(inputState);
    _inputStates.free(gameInputState);
}

void Server::deleteRobotWithPlayerId(uint8_t playerID)
{
    for (Entity* e : _world.getPlayers())
    {
        PlayerController* pc = static_cast<PlayerController*>(e->getController());
        if (pc->getPlayerID() == playerID)
        {
            e->requestDeletion();
            break;
        }
    }
}

void Server::spawnEntityForPlayer(uint8_t playerId, std::string playerName)
{
    ClientProxy* client = NW_MGR_SERVER->getClientProxy(playerId);
    if (client == NULL)
    {
        return;
    }
    
    float spawnX = playerId == 1 ? CFG_MAIN._hideSpawnX : CFG_MAIN._jackieSpawnX;
    float spawnY = playerId == 1 ? CFG_MAIN._hideSpawnY : CFG_MAIN._jackieSpawnY;
    
    uint32_t key = playerId == 1 ? 'HIDE' : 'JCKE';
    EntityInstanceDef eid(_entityIDManager->getNextDynamicEntityID(), key, spawnX, spawnY);
    Entity* e = ENTITY_MAPPER.createEntity(&eid, true);
    PlayerController* pc = static_cast<PlayerController*>(e->getController());
    pc->setAddressHash(client->getMachineAddress()->getHash());
    pc->setPlayerName(playerName);
    pc->setPlayerID(playerId);
    
    _world.addEntity(e);
    
    NW_MGR_SERVER->registerEntity(e);
}

void Server::loadMap()
{
    std::vector<uint8_t> playerIDs;
    std::vector<std::string> playerNames;
    for (Entity* e : _world.getPlayers())
    {
        PlayerController* pc = static_cast<PlayerController*>(e->getController());
        playerIDs.push_back(pc->getPlayerID());
        playerNames.push_back(pc->getPlayerName());
    }
    
    for (Entity* e : _world.getDynamicEntities())
    {
        NW_MGR_SERVER->deregisterEntity(e);
    }
    
    _world.loadMap(_map);
    
    for (Entity* e : _world.getDynamicEntities())
    {
        NW_MGR_SERVER->registerEntity(e);
    }
    
    NW_MGR_SERVER->setMap(_map);
    
    for (int i = 0; i < playerIDs.size() && i < playerNames.size(); ++i)
    {
        handleNewClient(playerIDs[i], playerNames[i]);
    }
}

void Server::handleDirtyStates(std::vector<Entity*>& entities)
{
    for (Entity* e : entities)
    {
        uint16_t dirtyState = e->getNetworkController()->getDirtyState();
        if (dirtyState > 0)
        {
            NW_MGR_SERVER->setStateDirty(e->getID(), dirtyState);
        }
    }
}

Server::Server() :
_timing(static_cast<Timing*>(INSTANCE_MGR.get(INSK_TIMING_SERVER))),
_entityIDManager(static_cast<EntityIDManager*>(INSTANCE_MGR.get(INSK_ENTITY_ID_MANAGER_SERVER))),
_world(WorldFlag_Server),
_map(0)
{
    _timing->reset();
    
    NetworkManagerServer::create(new SocketServerHelper(CFG_MAIN._serverPort, CFG_MAIN._maxNumPlayers, NW_MGR_SERVER_CALLBACKS), SERVER_CALLBACKS);
    assert(NW_MGR_SERVER != NULL);
}

Server::~Server()
{
    NetworkManagerServer::destroy();
}
