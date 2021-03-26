//
//  GameServerEngineState.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

void cb_server_onEntityRegistered(Entity* e)
{
    ENGINE_STATE_GAME_SRVR.world().addNetworkEntity(e);
    
    if (e->isPlayer() && e->metadata().getUInt("playerID", 0) == 1)
    {
        uint32_t entityLayoutKey = e->dataField("entityLayoutKey").valueUInt32();
        EntityLayout* elm = INST_REG.get<EntityLayout>(INSK_ELM_SRVR);
        EntityLayoutDef& eld = elm->entityLayoutDef(entityLayoutKey);
        EntityLayoutLoader::loadEntityLayout(eld, true);
        ENGINE_STATE_GAME_SRVR.populateFromEntityLayout(eld);
    }
}

void cb_server_onEntityDeregistered(Entity* e)
{
    bool needsRestart = false;
    
    if (e->isPlayer())
    {
        uint8_t playerID = e->metadata().getUInt("playerID");
        needsRestart = NW_SRVR->getClientProxy(playerID) != NULL;
    }
    
    ENGINE_STATE_GAME_SRVR.world().removeNetworkEntity(e);
    
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
    
    world().reset();
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
    for (auto& e : world().getNetworkEntities())
    {
        NW_SRVR->deregisterEntity(e);
    }
    
    world().populateFromEntityLayout(eld);
    
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
    std::vector<Entity*>& players = world().getPlayers();
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

World& GameServerEngineState::world()
{
    return *_world;
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
    for (Entity* e : world().getPlayers())
    {
        NW_SRVR->removeProcessedMovesForPlayer(e->metadata().getUInt("playerID"));
    }
    NW_SRVR->onMovesProcessed(moveCount);
    
    NW_SRVR->sendOutgoingPackets();
}

void GameServerEngineState::updateWorld(int moveIndex)
{
    for (Entity* e : world().getPlayers())
    {
        PlayerController* ec = e->controller<PlayerController>();
        assert(ec != NULL);
        
        ClientProxy* cp = NW_SRVR->getClientProxy(e->metadata().getUInt("playerID"));
        assert(cp != NULL);
        
        MoveList& ml = cp->getUnprocessedMoveList();
        Move* m = ml.getMoveAtIndex(moveIndex);
        assert(m != NULL);
        
        ec->processInput(m->inputState(), false);
        ml.markMoveAsProcessed(m);
        cp->setLastMoveTimestampDirty(true);
    }
    
    world().stepPhysics(INST_REG.get<TimeTracker>(INSK_TIME_SRVR));
    std::vector<Entity*> toDelete = world().update();
    for (Entity* e : toDelete)
    {
        bool exitImmediately = e->isPlayer();
        NW_SRVR->deregisterEntity(e);
        if (exitImmediately)
        {
            return;
        }
    }
    
    handleDirtyStates(world().getPlayers());
    handleDirtyStates(world().getNetworkEntities());
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
    
    uint32_t spawnX = playerID == 1 ? rand() % 24 + 6 : rand() % 24 + 58;
    uint32_t spawnY = playerID == 1 ? rand() % 16 + 6 : rand() % 6 + 6;
    
    uint32_t key = playerID == 1 ? 'HIDE' : 'JCKE';
    uint32_t networkID = INST_REG.get<EntityIDManager>(INSK_EID_SRVR)->getNextPlayerEntityID();
    EntityInstanceDef eid(networkID, key, spawnX, spawnY, true);
    Entity* e = ENTITY_MGR.createEntity(eid);
    e->dataField("username").valueString() = username;
    e->dataField("userAddress").valueString() = cp->getSocketAddress()->toString();
    if (playerID == 1)
    {
        e->dataField("entityLayoutKey").valueUInt32() = 'LYT1';
    }
    
    NW_SRVR->registerEntity(e);
}

void GameServerEngineState::removePlayer(uint8_t playerID)
{
    for (Entity* e : world().getPlayers())
    {
        if (e->metadata().getUInt("playerID") == playerID)
        {
            NW_SRVR->deregisterEntity(e);
            break;
        }
    }
}

GameServerEngineState::GameServerEngineState() : State<Engine>(),
_world(new World()),
_isRestarting(false)
{
    // Empty
}
