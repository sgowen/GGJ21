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
#include "HidePlayerController.hpp"
#include "EntityManager.hpp"
#include "EntityLayoutManager.hpp"
#include "InstanceRegistry.hpp"
#include "MainConfig.hpp"
#include "Network.hpp"
#include "StringUtil.hpp"

#include <ctime>
#include <assert.h>

void cb_server_onEntityRegistered(Entity* e)
{
    ENGINE_STATE_GAME_SRVR.getWorld().addNetworkEntity(e);
    
    if (e->controller()->getRTTI().isExactly(HidePlayerController::rtti))
    {
        HidePlayerController* c = static_cast<HidePlayerController*>(e->controller());
        uint32_t key = c->getEntityLayoutKey();
        EntityLayoutDef& eld = ENTITY_LAYOUT_MGR.findEntityLayoutDef(key);
        ENGINE_STATE_GAME_SRVR.loadEntityLayout(eld);
    }
}

void cb_server_onEntityDeregistered(Entity* e)
{
    bool needsRestart = false;
    
    if (e->controller()->getRTTI().isDerivedFrom(PlayerController::rtti))
    {
        PlayerController* c = static_cast<PlayerController*>(e->controller());
        assert(c != NULL);
        
        std::vector<PlayerDef>& players = ENGINE_STATE_GAME_SRVR.getPlayers();
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

InputState* cb_server_handleInputStateCreation()
{
    return ENGINE_STATE_GAME_SRVR.handleInputStateCreation();
}

void cb_server_handleInputStateRelease(InputState* inputState)
{
    ENGINE_STATE_GAME_SRVR.handleInputStateRelease(inputState);
}

#define GAME_ENGINE_SERVER_CBS cb_server_onEntityRegistered, cb_server_onEntityDeregistered, cb_server_handleNewClient, cb_server_handleLostClient, cb_server_handleInputStateCreation, cb_server_handleInputStateRelease

void GameServerEngineState::enter(Engine* e)
{
    NetworkServer::create(CFG_MAIN._serverPort, CFG_MAIN._maxNumPlayers, GAME_ENGINE_SERVER_CBS);
    assert(NW_SRVR != NULL);
    
    if (!NW_SRVR->connect())
    {
        e->revertToPreviousState();
        return;
    }
    
    if (!_args.hasValues())
    {
        ENTITY_MGR.initWithJSONFile("assets/json/entities.json");
        ENTITY_LAYOUT_MGR.initWithJSONFile("assets/json/layouts.json");
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
    _world.clearLayout();
    _world.clearNetwork();
    
    NetworkServer::destroy();
    
    if (!_args.hasValues())
    {
        ENTITY_MGR.clear();
        ENTITY_LAYOUT_MGR.clear();
    }
}

void GameServerEngineState::handleNewClient(std::string username, uint8_t playerID)
{
    _players.emplace_back(username, playerID);
    
    registerPlayer(username, playerID);
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
    
    if (NW_SRVR->getNumPlayersConnected() == 0)
    {
        resetWorld();
    }
}

InputState* GameServerEngineState::handleInputStateCreation()
{
    InputState* ret = _poolGameInputState.obtain();
    ret->reset();
    
    return ret;
}

void GameServerEngineState::handleInputStateRelease(InputState* inputState)
{
    GameInputState* gameInputState = static_cast<GameInputState*>(inputState);
    _poolGameInputState.free(gameInputState);
}

void GameServerEngineState::resetWorld()
{
    _world.clearLayout();
    
    std::vector<Entity*>& networkEntities = _world.getNetworkEntities();
    while (!networkEntities.empty())
    {
        NW_SRVR->deregisterEntity(networkEntities.front());
    }
}

void GameServerEngineState::loadEntityLayout(EntityLayoutDef& eld)
{
    resetWorld();
    
    ENTITY_LAYOUT_MGR.loadEntityLayout(eld, INST_REG.get<EntityIDManager>(INSK_EID_SRVR));
    _world.populateFromEntityLayout(eld, true);
    
    for (auto& eid : eld._entitiesNetwork)
    {
        NW_SRVR->registerEntity(ENTITY_MGR.createEntity(eid, true));
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
    
    for (int i = 0; i < _players.size(); ++i)
    {
        registerPlayer(_players[i]._username, _players[i]._playerID);
    }
}

std::vector<PlayerDef>& GameServerEngineState::getPlayers()
{
    return _players;
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
    uint32_t expectedMoveIndex = NW_SRVR->getNumMovesProcessed();
    int validMoveCount = 0;
    for (int i = 0; i < moveCount; ++i)
    {
        bool isMoveValid = true;
        for (Entity* e : _world.getPlayers())
        {
            PlayerController* c = static_cast<PlayerController*>(e->controller());
            assert(c != NULL);

            ClientProxy* cp = NW_SRVR->getClientProxy(c->getPlayerID());
            assert(cp != NULL);

            MoveList& ml = cp->getUnprocessedMoveList();
            Move* m = ml.getMoveAtIndex(i);
            assert(m != NULL);

            if (expectedMoveIndex != m->getIndex())
            {
                isMoveValid = false;
                break;
            }
        }

        if (isMoveValid)
        {
            ++validMoveCount;
            ++expectedMoveIndex;
        }
    }
    
    assert(moveCount == validMoveCount);
    for (int i = 0; i < validMoveCount; ++i)
    {
        updateWorld(i);
    }
    
    for (Entity* e : _world.getPlayers())
    {
        PlayerController* c = static_cast<PlayerController*>(e->controller());
        assert(c != NULL);
        
        ClientProxy* cp = NW_SRVR->getClientProxy(c->getPlayerID());
        assert(cp != NULL);
        
        MoveList& ml = cp->getUnprocessedMoveList();
        ml.removeProcessedMoves(cp->getUnprocessedMoveList().getLastProcessedMoveTimestamp(), cb_server_handleInputStateRelease);
    }
    
    NW_SRVR->sendOutgoingPackets();
}

void GameServerEngineState::updateWorld(int moveIndex)
{
    for (Entity* e : _world.getPlayers())
    {
        PlayerController* c = static_cast<PlayerController*>(e->controller());
        assert(c != NULL);
        
        ClientProxy* cp = NW_SRVR->getClientProxy(c->getPlayerID());
        assert(cp != NULL);
        
        MoveList& ml = cp->getUnprocessedMoveList();
        Move* m = ml.getMoveAtIndex(moveIndex);
        assert(m != NULL);
        
        c->processInput(m->inputState(), false);
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
        NW_SRVR->deregisterEntity(e);
    }
    
    handleDirtyStates(_world.getPlayers());
    handleDirtyStates(_world.getNetworkEntities());
    
    NW_SRVR->onMoveProcessed();
}

void GameServerEngineState::registerPlayer(std::string username, uint8_t playerID)
{
    ClientProxy* cp = NW_SRVR->getClientProxy(playerID);
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
    
    NW_SRVR->registerEntity(e);
}

void GameServerEngineState::removePlayer(uint8_t playerID)
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
            NW_SRVR->deregisterEntity(e);
            break;
        }
    }
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

GameServerEngineState::GameServerEngineState() : State<Engine>(),
_world(),
_isRestarting(false)
{
    // Empty
}
