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
#include "TimeTracker.hpp"
#include "EntityIDManager.hpp"
#include "NetworkManagerServer.hpp"
#include "EntityRegistry.hpp"
#include "SocketAddress.hpp"
#include "PlayerController.hpp"
#include "HidePlayerController.hpp"
#include "EntityManager.hpp"
#include "EntityLayoutManager.hpp"
#include "InstanceRegistry.hpp"
#include "MainConfig.hpp"
#include "Network.hpp"

#include <ctime>
#include <assert.h>

Server* Server::s_instance = NULL;

void Server::create()
{
    assert(s_instance == NULL);
    
    s_instance = new Server();
}

Server * Server::getInstance()
{
    return s_instance;
}

void Server::destroy()
{
    assert(s_instance != NULL);
    
    delete s_instance;
    s_instance = NULL;
}

void Server::handleNewClient(std::string username, uint8_t playerID)
{
    _players.emplace_back(username, playerID);
    
    registerPlayer(username, playerID);
}

void Server::handleLostClient(ClientProxy& cp, uint8_t index)
{
    if (index >= 1)
    {
        uint8_t playerID = cp.getPlayerID(index);
        removePlayer(playerID);
    }
    else
    {
        for (int i = 0; i < cp.getNumPlayers(); ++i)
        {
            uint8_t playerID = cp.getPlayerID(i);
            removePlayer(playerID);
        }
    }
}

InputState* Server::handleInputStateCreation()
{
    InputState* ret = _poolGameInputState.obtain();
    ret->reset();
    
    return ret;
}

void Server::handleInputStateRelease(InputState* inputState)
{
    GameInputState* gameInputState = static_cast<GameInputState*>(inputState);
    _poolGameInputState.free(gameInputState);
}

void Server::loadEntityLayout(EntityLayoutDef& eld)
{
    std::vector<Entity*>& networkEntities = _world.getNetworkEntities();
    while (!networkEntities.empty())
    {
        NW_MGR_SRVR->deregisterEntity(networkEntities.front());
    }
    
    ENTITY_LAYOUT_MGR.loadEntityLayout(eld, INST_REG.get<EntityIDManager>(INSK_EID_SRVR));
    _world.populateFromEntityLayout(eld, true);
    
    for (auto& eid : eld._entitiesNetwork)
    {
        NW_MGR_SRVR->registerEntity(ENTITY_MGR.createEntity(eid, true));
    }
}

void Server::restart()
{
    if (_isRestarting)
    {
        return;
    }
    
    _isRestarting = true;
    std::vector<Entity*>& players = _world.getPlayers();
    while (!players.empty())
    {
        Entity* e = players.front();
        NW_MGR_SRVR->deregisterEntity(e);
    }
    _isRestarting = false;
    
    for (int i = 0; i < _players.size(); ++i)
    {
        registerPlayer(_players[i]._username, _players[i]._playerID);
    }
}

std::vector<PlayerDef>& Server::getPlayers()
{
    return _players;
}

void Server::update()
{
    INST_REG.get<TimeTracker>(INSK_TIME_SRVR)->onFrame();
    
    NW_MGR_SRVR->processIncomingPackets();
    
    // TODO, this attempts to figure out an average
    // really, what we need to do is lockstep
    // process moves that correspond to the same server timestamp
    int moveCount = NW_MGR_SRVR->getMoveCount();
    for (int i = 0; i < moveCount; ++i)
    {
        for (Entity* e : _world.getPlayers())
        {
            PlayerController* c = static_cast<PlayerController*>(e->controller());
            assert(c != NULL);
            
            ClientProxy* cp = NW_MGR_SRVR->getClientProxy(c->getPlayerID());
            assert(cp != NULL);
            
            MoveList& ml = cp->getUnprocessedMoveList();
            Move* m = ml.getMoveAtIndex(i);
            if (m == NULL)
            {
                continue;
            }
            
            c->processInput(m->inputState());
            ml.markMoveAsProcessed(m);
            cp->setLastMoveTimestampDirty(true);
        }
        
        _world.stepPhysics(INST_REG.get<TimeTracker>(INSK_TIME_SRVR));
        
        std::vector<Entity*> toDelete;
        for (Entity* e : _world.getPlayers())
        {
            e->update();
            if (e->isRequestingDeletion())
            {
                toDelete.push_back(e);
            }
        }
        for (Entity* e : _world.getNetworkEntities())
        {
            e->update();
            if (e->isRequestingDeletion())
            {
                toDelete.push_back(e);
            }
        }
        for (Entity* e : toDelete)
        {
            NW_MGR_SRVR->deregisterEntity(e);
        }
        
        handleDirtyStates(_world.getPlayers());
        handleDirtyStates(_world.getNetworkEntities());
    }
    
    removeProcessedMoves();
    
    NW_MGR_SRVR->sendOutgoingPackets();
}

World& Server::getWorld()
{
    return _world;
}

void Server::registerPlayer(std::string username, uint8_t playerID)
{
    ClientProxy* cp = NW_MGR_SRVR->getClientProxy(playerID);
    assert(cp != NULL);
    
    float spawnX = playerID == 1 ? rand() % 24 + 6 : rand() % 24 + 58;
    float spawnY = playerID == 1 ? rand() % 16 + 6 : rand() % 6 + 6;
    
    uint32_t key = playerID == 1 ? 'HIDE' : 'JCKE';
    uint32_t networkID = INST_REG.get<EntityIDManager>(INSK_EID_SRVR)->getNextPlayerEntityID();
    EntityInstanceDef eid(networkID, key, spawnX, spawnY);
    Entity* e = ENTITY_MGR.createEntity(eid, true);
    PlayerController* c = static_cast<PlayerController*>(e->controller());
    c->setUsername(username);
    c->setUserAddress(cp->getSocketAddress()->toString());
    c->setPlayerID(playerID);
    if (c->getRTTI().isExactly(HidePlayerController::rtti))
    {
        HidePlayerController* hpc = static_cast<HidePlayerController*>(c);
        hpc->setEntityLayoutKey('LO01');
    }
    
    NW_MGR_SRVR->registerEntity(e);
}

void Server::removePlayer(uint8_t playerID)
{
    for (std::vector<PlayerDef>::iterator i = _players.begin(); i != _players.end(); ++i)
    {
        PlayerDef& pd = *i;
        if (pd._playerID == playerID)
        {
            _players.erase(i);
            break;
        }
    }
    
    for (Entity* e : _world.getPlayers())
    {
        PlayerController* c = static_cast<PlayerController*>(e->controller());
        if (c->getPlayerID() == playerID)
        {
            NW_MGR_SRVR->deregisterEntity(e);
            break;
        }
    }
}

void Server::handleDirtyStates(std::vector<Entity*>& entities)
{
    for (Entity* e : entities)
    {
        uint8_t dirtyState = e->networkController()->refreshDirtyState();
        if (dirtyState > 0)
        {
            NW_MGR_SRVR->setStateDirty(e->getID(), dirtyState);
        }
    }
}

void cb_server_onEntityRegistered(Entity* e)
{
    Server::getInstance()->getWorld().addNetworkEntity(e);
    
    if (e->controller()->getRTTI().isExactly(HidePlayerController::rtti))
    {
        HidePlayerController* c = static_cast<HidePlayerController*>(e->controller());
        uint32_t key = c->getEntityLayoutKey();
        EntityLayoutDef& eld = ENTITY_LAYOUT_MGR.findEntityLayoutDef(key);
        Server::getInstance()->loadEntityLayout(eld);
    }
}

void cb_server_onEntityDeregistered(Entity* e)
{
    bool needsRestart = false;
    
    if (e->controller()->getRTTI().isDerivedFrom(PlayerController::rtti))
    {
        PlayerController* c = static_cast<PlayerController*>(e->controller());
        assert(c != NULL);
        
        std::vector<PlayerDef>& players = Server::getInstance()->getPlayers();
        for (std::vector<PlayerDef>::iterator i = players.begin(); i != players.end(); ++i)
        {
            PlayerDef& pd = *i;
            if (pd._playerID == c->getPlayerID())
            {
                needsRestart = true;
                break;
            }
        }
    }
    
    Server::getInstance()->getWorld().removeNetworkEntity(e);
    
    if (needsRestart)
    {
        Server::getInstance()->restart();
    }
}

void cb_server_handleNewClient(std::string username, uint8_t playerID)
{
    Server::getInstance()->handleNewClient(username, playerID);
}

void cb_server_handleLostClient(ClientProxy& cp, uint8_t index)
{
    Server::getInstance()->handleLostClient(cp, index);
}

InputState* cb_server_handleInputStateCreation()
{
    return Server::getInstance()->handleInputStateCreation();
}

void cb_server_handleInputStateRelease(InputState* inputState)
{
    Server::getInstance()->handleInputStateRelease(inputState);
}

void Server::removeProcessedMoves()
{
    for (Entity* e : _world.getPlayers())
    {
        PlayerController* c = static_cast<PlayerController*>(e->controller());
        assert(c != NULL);
        
        ClientProxy* cp = NW_MGR_SRVR->getClientProxy(c->getPlayerID());
        assert(cp != NULL);
        
        MoveList& moveList = cp->getUnprocessedMoveList();
        moveList.removeProcessedMoves(cp->getUnprocessedMoveList().getLastProcessedMoveTimestamp(), cb_server_handleInputStateRelease);
    }
}

#define SERVER_CBS cb_server_onEntityRegistered, cb_server_onEntityDeregistered, cb_server_handleNewClient, cb_server_handleLostClient, cb_server_handleInputStateCreation, cb_server_handleInputStateRelease

Server::Server() :
_world(),
_isRestarting(false)
{
    NetworkManagerServer::create(CFG_MAIN._serverPort, CFG_MAIN._maxNumPlayers, SERVER_CBS);
    assert(NW_MGR_SRVR != NULL);
}

Server::~Server()
{
    NetworkManagerServer::destroy();
}
