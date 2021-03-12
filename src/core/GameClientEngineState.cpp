//
//  GameClientEngineState.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

void cb_client_onEntityRegistered(Entity* e)
{
    ENGINE_STATE_GAME_CLNT.world().addNetworkEntity(e);
    
    if (e->controller()->getRTTI().isDerivedFrom(HideController::rtti))
    {
        uint32_t entityLayoutKey = e->dataField("entityLayoutKey").valueUInt32();
        EntityLayout* elm = INST_REG.get<EntityLayout>(INSK_ELM_CLNT);
        EntityLayoutDef& eld = elm->entityLayoutDef(entityLayoutKey);
        EntityLayoutLoader::loadEntityLayout(eld, false);
        ENGINE_STATE_GAME_CLNT.world().populateFromEntityLayout(eld);
    }
}

void cb_client_onEntityDeregistered(Entity* e)
{
    ENGINE_STATE_GAME_CLNT.world().removeNetworkEntity(e);
}

void cb_client_removeProcessedMoves(float lastMoveProcessedOnServerTimestamp)
{
    INPUT_GAME.moveList().removeProcessedMovesAtTimestamp(lastMoveProcessedOnServerTimestamp, GameInputManager::cb_inputStateRelease);
}

void cb_client_onPlayerWelcomed(uint8_t playerID)
{
    INPUT_GAME.inputState()->activateNextPlayer(playerID);
}

#define GAME_ENGINE_CLIENT_CBS cb_client_onEntityRegistered, cb_client_onEntityDeregistered, cb_client_removeProcessedMoves, cb_client_onPlayerWelcomed

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
    world().reset();
    
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
            world().recallCache();
            
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
    
    NW_CLNT->sendOutgoingPackets(INPUT_GAME.moveList());
}

Entity* GameClientEngineState::getControlledPlayer()
{
    uint8_t playerID = INPUT_GAME.inputState()->playerInputState(0)._playerID;
    Entity* ret = NULL;
    
    for (Entity* e : ENGINE_STATE_GAME_CLNT.world().getPlayers())
    {
        if (playerID == e->metadata().getUInt("playerID"))
        {
            ret = e;
            break;
        }
    }
    
    return ret;
}

World& GameClientEngineState::world()
{
    return *_world;
}

void GameClientEngineState::updateWorld(const Move& move, bool isLive)
{
    for (Entity* e : world().getPlayers())
    {
        PlayerController* ec = e->controller<PlayerController>();
        assert(ec != NULL);
        ec->processInput(move.inputState(), isLive);
    }
    
    world().stepPhysics(INST_REG.get<TimeTracker>(INSK_TIME_CLNT));
    world().update();
    
    NW_CLNT->onMoveProcessed();
}

GameClientEngineState::GameClientEngineState() : EngineState("data/json/assets_game.json", "data/json/renderer_game.json", GameRenderer::render),
_world(new World())
{
    // Empty
}
