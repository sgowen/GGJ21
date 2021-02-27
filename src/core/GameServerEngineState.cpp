//
//  GameServerEngineState.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GameServerEngineState.hpp"

#include "Engine.hpp"
#include "ClientProxy.hpp"
#include "Entity.hpp"
#include "TimeTracker.hpp"
#include "EntityIDManager.hpp"
#include "NetworkServer.hpp"
#include "EntityRegistry.hpp"
#include "SocketAddress.hpp"
#include "PlayerController.hpp"
#include "HideController.hpp"
#include "EntityManager.hpp"
#include "EntityLayout.hpp"
#include "InstanceRegistry.hpp"
#include "MainConfig.hpp"
#include "Network.hpp"
#include "StringUtil.hpp"
#include "rapidjson/EntityLayoutLoader.hpp"

#include <ctime>
#include <assert.h>

void cb_server_onEntityRegistered(Entity* e)
{
    ENGINE_STATE_GAME_SRVR.getWorld().addNetworkEntity(e);
    
    if (e->entityDef()._data.getUInt("playerID", 0) == 1)
    {
        HideController* ec = e->controller<HideController>();
        uint32_t key = ec->getEntityLayoutKey();
        EntityLayout* elm = INST_REG.get<EntityLayout>(INSK_ELM_SRVR);
        EntityLayoutDef& eld = elm->entityLayoutDef(key);
        EntityLayoutLoader::loadEntityLayout(eld, true);
        ENGINE_STATE_GAME_SRVR.populateFromEntityLayout(eld);
    }
}

void cb_server_onEntityDeregistered(Entity* e)
{
    bool needsRestart = false;
    
    if (e->isPlayer())
    {
        PlayerController* ec = e->controller<PlayerController>();
        assert(ec != NULL);
        
        needsRestart = NW_SRVR->getClientProxy(ec->getPlayerID()) != NULL;
    }
    
    ENGINE_STATE_GAME_SRVR.getWorld().removeNetworkEntity(e);
    
    if (needsRestart)
    {
        ENGINE_STATE_GAME_SRVR.restart();
    }
}

void cb_server_handleNewClient(std::string username, uint8_t playerID)
{
    ENGINE_STATE_GAME_SRVR.handleNewClient(username, playerID);
}

void cb_server_handleLostClient(ClientProxy& cp, uint8_t localPlayerIndex)
{
    ENGINE_STATE_GAME_SRVR.handleLostClient(cp, localPlayerIndex);
}

#define GAME_ENGINE_SERVER_CBS cb_server_onEntityRegistered, cb_server_onEntityDeregistered, cb_server_handleNewClient, cb_server_handleLostClient

void GameServerEngineState::enter(Engine* e)
{
    NetworkServer::create(CFG_MAIN.serverPort(), CFG_MAIN.maxNumPlayers(), GAME_ENGINE_SERVER_CBS);
    assert(NW_SRVR != NULL);
    
    if (!NW_SRVR->connect())
    {
        e->revertToPreviousState();
        return;
    }
}

void GameServerEngineState::execute(Engine* e)
{
    switch (e->requestedStateAction())
    {
        case ERSA_UPDATE:
            update(e);
            break;
        default:
            break;
    }
}

void GameServerEngineState::exit(Engine* e)
{
    NetworkServer::destroy();
    
    _world.reset();
}

void GameServerEngineState::handleNewClient(std::string username, uint8_t playerID)
{
    addPlayer(username, playerID);
}

void GameServerEngineState::handleLostClient(ClientProxy& cp, uint8_t localPlayerIndex)
{
    if (localPlayerIndex >= 1)
    {
        uint8_t playerID = cp.getPlayerID(localPlayerIndex);
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

void GameServerEngineState::populateFromEntityLayout(EntityLayoutDef& eld)
{
    for (auto& e : _world.getNetworkEntities())
    {
        NW_SRVR->deregisterEntity(e);
    }
    
    _world.populateFromEntityLayout(eld);
    
    for (auto& eid : eld._entitiesNetwork)
    {
        NW_SRVR->registerEntity(ENTITY_MGR.createEntity(eid));
    }
}

void GameServerEngineState::restart()
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
        NW_SRVR->deregisterEntity(e);
    }
    _isRestarting = false;
    
    const std::map<int, ClientProxy*>& playerIDToClientMap = NW_SRVR->playerIDToClientMap();
    for (auto& pair : playerIDToClientMap)
    {
        ClientProxy* cp = pair.second;
        assert(cp != NULL);
        addPlayer(cp->getUsername(), pair.first);
    }
}

World& GameServerEngineState::getWorld()
{
    return _world;
}

void GameServerEngineState::update(Engine* e)
{
    INST_REG.get<TimeTracker>(INSK_TIME_SRVR)->onFrame();
    
    NW_SRVR->processIncomingPackets();
    int moveCount = NW_SRVR->getMoveCount();
    for (int i = 0; i < moveCount; ++i)
    {
        updateWorld(i);
    }
    for (Entity* e : _world.getPlayers())
    {
        NW_SRVR->removeProcessedMovesForPlayer(e->controller<PlayerController>()->getPlayerID());
    }
    NW_SRVR->onMovesProcessed(moveCount);
    
    NW_SRVR->sendOutgoingPackets();
}

void GameServerEngineState::updateWorld(int moveIndex)
{
    for (Entity* e : _world.getPlayers())
    {
        PlayerController* ec = e->controller<PlayerController>();
        assert(ec != NULL);
        
        ClientProxy* cp = NW_SRVR->getClientProxy(ec->getPlayerID());
        assert(cp != NULL);
        
        MoveList& ml = cp->getUnprocessedMoveList();
        Move* m = ml.getMoveAtIndex(moveIndex);
        assert(m != NULL);
        
        ec->processInput(m->inputState(), false);
        ml.markMoveAsProcessed(m);
        cp->setLastMoveTimestampDirty(true);
    }
    
    _world.stepPhysics(INST_REG.get<TimeTracker>(INSK_TIME_SRVR));
    std::vector<Entity*> toDelete = _world.update();
    for (Entity* e : toDelete)
    {
        bool exitImmediately = e->isPlayer();
        NW_SRVR->deregisterEntity(e);
        if (exitImmediately)
        {
            return;
        }
    }
    
    handleDirtyStates(_world.getPlayers());
    handleDirtyStates(_world.getNetworkEntities());
}

void GameServerEngineState::handleDirtyStates(std::vector<Entity*>& entities)
{
    for (Entity* e : entities)
    {
        uint8_t dirtyState = e->networkController()->refreshDirtyState();
        if (dirtyState > 0)
        {
            NW_SRVR->setStateDirty(e->getID(), dirtyState);
        }
    }
}

void GameServerEngineState::addPlayer(std::string username, uint8_t playerID)
{
    ClientProxy* cp = NW_SRVR->getClientProxy(playerID);
    assert(cp != NULL);
    
    float spawnX = playerID == 1 ? rand() % 24 + 6 : rand() % 24 + 58;
    float spawnY = playerID == 1 ? rand() % 16 + 6 : rand() % 6 + 6;
    
    uint32_t key = playerID == 1 ? 'HIDE' : 'JCKE';
    uint32_t networkID = INST_REG.get<EntityIDManager>(INSK_EID_SRVR)->getNextPlayerEntityID();
    EntityInstanceDef eid(networkID, key, spawnX, spawnY, true);
    Entity* e = ENTITY_MGR.createEntity(eid);
    PlayerController* ec = e->controller<PlayerController>();
    ec->setUsername(username);
    ec->setUserAddress(cp->getSocketAddress()->toString());
    ec->setPlayerID(playerID);
    if (ec->getRTTI().isExactly(HideController::rtti))
    {
        e->controller<HideController>()->setEntityLayoutKey('LYT1');
    }
    
    NW_SRVR->registerEntity(e);
}

void GameServerEngineState::removePlayer(uint8_t playerID)
{
    for (Entity* e : _world.getPlayers())
    {
        PlayerController* ec = e->controller<PlayerController>();
        if (ec->getPlayerID() == playerID)
        {
            NW_SRVR->deregisterEntity(e);
            break;
        }
    }
}

GameServerEngineState::GameServerEngineState() : State<Engine>(),
_world(),
_isRestarting(false)
{
    // Empty
}
