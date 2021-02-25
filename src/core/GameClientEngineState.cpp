//
//  GameClientEngineState.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GameClientEngineState.hpp"

#include "Engine.hpp"
#include "TimeTracker.hpp"
#include "Move.hpp"
#include "AssetManager.hpp"
#include "GameInputManager.hpp"
#include "AudioEngineFactory.hpp"
#include "PlayerController.hpp"
#include "Entity.hpp"
#include "StringUtil.hpp"
#include "MathUtil.hpp"
#include "NetworkClient.hpp"
#include "GameInputState.hpp"
#include "EntityRegistry.hpp"
#include "Assets.hpp"
#include "EntityManager.hpp"
#include "EntityLayoutManager.hpp"
#include "MainConfig.hpp"
#include "InstanceRegistry.hpp"
#include "Config.hpp"
#include "Macros.hpp"
#include "Network.hpp"
#include "HideController.hpp"
#include "GameRenderer.hpp"
#include "Renderer.hpp"
#include "AssetsLoader.hpp"

#include <stdlib.h>
#include <assert.h>

void cb_client_onEntityRegistered(Entity* e)
{
    ENGINE_STATE_GAME_CLNT.getWorld().addNetworkEntity(e);
    
    if (e->controller()->getRTTI().isExactly(HideController::rtti))
    {
        HideController* ec = e->controller<HideController>();
        uint32_t key = ec->getEntityLayoutKey();
        EntityLayoutManager* elm = INST_REG.get<EntityLayoutManager>(INSK_ELM_CLNT);
        EntityLayoutDef& eld = elm->findEntityLayoutDef(key);
        elm->loadEntityLayout(eld);
        ENGINE_STATE_GAME_CLNT.getWorld().populateFromEntityLayout(eld);
    }
}

void cb_client_onEntityDeregistered(Entity* e)
{
    ENGINE_STATE_GAME_CLNT.getWorld().removeNetworkEntity(e);
}

void cb_client_handleInputStateRelease(InputState* inputState)
{
    GameInputState* gis = static_cast<GameInputState*>(inputState);
    INPUT_GAME.free(gis);
}

void cb_client_removeProcessedMoves(float lastMoveProcessedOnServerTimestamp)
{
    INPUT_GAME.moveList().removeProcessedMoves(lastMoveProcessedOnServerTimestamp, cb_client_handleInputStateRelease);
}

MoveList& cb_client_getMoveList()
{
    return INPUT_GAME.moveList();
}

void cb_client_onPlayerWelcomed(uint8_t playerID)
{
    INPUT_GAME.inputState()->activateNextPlayer(playerID);
}

#define GAME_ENGINE_CLIENT_CBS cb_client_onEntityRegistered, cb_client_onEntityDeregistered, cb_client_removeProcessedMoves, cb_client_getMoveList, cb_client_onPlayerWelcomed

void GameClientEngineState::enter(Engine* e)
{
    EngineState::enter(e);
    
    std::string serverIPAddress;
    uint16_t port;
    if (_args.hasValue(ARG_IP_ADDRESS))
    {
        serverIPAddress = _args.getString(ARG_IP_ADDRESS);
        port = CFG_MAIN._clientPortJoin;
    }
    else
    {
        serverIPAddress = "localhost";
        port = CFG_MAIN._clientPortHost;
    }
    
    NetworkClient::create(StringUtil::format("%s:%d", serverIPAddress.c_str(), CFG_MAIN._serverPort), _args.getString(ARG_USERNAME), port, GAME_ENGINE_CLIENT_CBS);
    assert(NW_CLNT != NULL);
    
    if (!NW_CLNT->connect())
    {
        e->revertToPreviousState();
        return;
    }
    
    AUDIO_ENGINE.playMusic(0.1f, true);
}

void GameClientEngineState::exit(Engine* e)
{
    EngineState::exit(e);
    
    if (NW_CLNT != NULL)
    {
        NetworkClient::destroy();
    }
    _world.reset();
    
    INPUT_GAME.reset();
}

void GameClientEngineState::update(Engine* e)
{
    EngineState::update(e);
    
    INST_REG.get<TimeTracker>(INSK_TIME_CLNT)->onFrame();
    
    if (INPUT_GAME.update() == GIMS_EXIT ||
        NW_CLNT->processIncomingPackets() == NWCS_DISCONNECTED)
    {
        e->revertToPreviousState();
        return;
    }
    
    if (getControlledPlayer() != NULL)
    {
        MoveList& ml = INPUT_GAME.moveList();
        if (NW_CLNT->hasReceivedNewState())
        {
            _world.recallCache();
            
            if (CFG_MAIN._networkLoggingEnabled)
            {
                LOG("Client side prediction reprocessing %d moves", ml.getMoveCount());
            }
            
            for (const Move& m : ml)
            {
                updateWorld(m, false);
            }
        }
        if (ml.getMoveCount() < NW_CLNT_MAX_NUM_MOVES)
        {
            updateWorld(INPUT_GAME.sampleInputAsNewMove(), true);
        }
    }
    
    NW_CLNT->sendOutgoingPackets();
}

void GameClientEngineState::render(Engine* e)
{
    EngineState::render(e);
    
    GameRenderer::render(_renderer);
}

Entity* GameClientEngineState::getControlledPlayer()
{
    uint8_t playerID = INPUT_GAME.inputState()->getPlayerInputState(0).playerID();
    Entity* ret = NULL;
    
    for (Entity* e : ENGINE_STATE_GAME_CLNT._world.getPlayers())
    {
        PlayerController* ec = e->controller<PlayerController>();
        
        if (playerID == ec->getPlayerID())
        {
            ret = e;
            break;
        }
    }
    
    return ret;
}

World& GameClientEngineState::getWorld()
{
    return _world;
}

void GameClientEngineState::updateWorld(const Move& move, bool isLive)
{
    for (Entity* e : _world.getPlayers())
    {
        PlayerController* ec = e->controller<PlayerController>();
        assert(ec != NULL);
        ec->processInput(move.inputState(), isLive);
    }
    
    _world.stepPhysics(INST_REG.get<TimeTracker>(INSK_TIME_CLNT));
    _world.update();
    
    NW_CLNT->onMoveProcessed();
}

GameClientEngineState::GameClientEngineState() : EngineState("data/json/assets_game.json", "data/json/renderer_game.json"),
_world()
{
    // Empty
}
