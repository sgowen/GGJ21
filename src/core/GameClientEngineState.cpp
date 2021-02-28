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
#include "AssetsManager.hpp"
#include "GameInputManager.hpp"
#include "AudioEngineFactory.hpp"
#include "PlayerController.hpp"
#include "Entity.hpp"
#include "StringUtil.hpp"
#include "MathUtil.hpp"
#include "NetworkClient.hpp"
#include "InputState.hpp"
#include "EntityRegistry.hpp"
#include "Assets.hpp"
#include "EntityManager.hpp"
#include "EntityLayout.hpp"
#include "MainConfig.hpp"
#include "InstanceRegistry.hpp"
#include "Config.hpp"
#include "Macros.hpp"
#include "Network.hpp"
#include "HideController.hpp"
#include "GameRenderer.hpp"
#include "Renderer.hpp"
#include "deps/rapidjson/EntityLayoutLoader.hpp"

#include <stdlib.h>
#include <assert.h>

void cb_client_onEntityRegistered(Entity* e)
{
    ENGINE_STATE_GAME_CLNT.getWorld().addNetworkEntity(e);
    
    if (e->controller()->getRTTI().isExactly(HideController::rtti))
    {
        HideController* ec = e->controller<HideController>();
        uint32_t key = ec->getEntityLayoutKey();
        EntityLayout* elm = INST_REG.get<EntityLayout>(INSK_ELM_CLNT);
        EntityLayoutDef& eld = elm->entityLayoutDef(key);
        EntityLayoutLoader::loadEntityLayout(eld, false);
        ENGINE_STATE_GAME_CLNT.getWorld().populateFromEntityLayout(eld);
    }
}

void cb_client_onEntityDeregistered(Entity* e)
{
    ENGINE_STATE_GAME_CLNT.getWorld().removeNetworkEntity(e);
}

void cb_client_removeProcessedMoves(float lastMoveProcessedOnServerTimestamp)
{
    INPUT_GAME.moveList().removeProcessedMovesAtTimestamp(lastMoveProcessedOnServerTimestamp, GameInputManager::cb_inputStateRelease);
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

void GameClientEngineState::onEnter(Engine* e)
{
    INPUT_GAME.setMatrix(&_renderer.matrix());
    
    std::string serverIPAddress;
    uint16_t port;
    if (_args.hasValue(ARG_IP_ADDRESS))
    {
        serverIPAddress = _args.getString(ARG_IP_ADDRESS);
        port = CFG_MAIN.clientPortJoin();
    }
    else
    {
        serverIPAddress = "localhost";
        port = CFG_MAIN.clientPortHost();
    }
    
    NetworkClient::create(StringUtil::format("%s:%d", serverIPAddress.c_str(), CFG_MAIN.serverPort()), _args.getString(ARG_USERNAME), port, GAME_ENGINE_CLIENT_CBS);
    assert(NW_CLNT != NULL);
    
    if (!NW_CLNT->connect())
    {
        e->revertToPreviousState();
        return;
    }
    
    AUDIO_ENGINE.playMusic(0.1f, true);
}

void GameClientEngineState::onExit(Engine* e)
{
    if (NW_CLNT != NULL)
    {
        NetworkClient::destroy();
    }
    _world.reset();
    
    INPUT_GAME.reset();
}

void GameClientEngineState::onUpdate(Engine* e)
{
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
            
            if (CFG_MAIN.networkLoggingEnabled())
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

Entity* GameClientEngineState::getControlledPlayer()
{
    uint8_t playerID = INPUT_GAME.inputState()->playerInputState(0)._playerID;
    Entity* ret = NULL;
    
    for (Entity* e : ENGINE_STATE_GAME_CLNT._world.getPlayers())
    {
        if (playerID == e->entityDef()._data.getUInt("playerID"))
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

GameClientEngineState::GameClientEngineState() : EngineState("data/json/assets_game.json", "data/json/renderer_game.json", GameRenderer::render),
_world()
{
    // Empty
}
