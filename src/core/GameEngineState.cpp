//
//  GameEngineState.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GameEngineState.hpp"

#include "Engine.hpp"
#include "TimeTracker.hpp"
#include "Server.hpp"
#include "Move.hpp"
#include "Assets.hpp"
#include "GameInputManager.hpp"
#include "GowAudioEngine.hpp"
#include "PlayerController.hpp"
#include "Entity.hpp"
#include "StringUtil.hpp"
#include "MathUtil.hpp"
#include "NetworkManagerClient.hpp"
#include "NetworkManagerServer.hpp"
#include "GameInputState.hpp"
#include "EntityRegistry.hpp"
#include "GowAudioEngine.hpp"
#include "Assets.hpp"
#include "EntityManager.hpp"
#include "EntityLayoutManager.hpp"
#include "MainConfig.hpp"
#include "InstanceRegistry.hpp"
#include "Config.hpp"
#include "Macros.hpp"
#include "Network.hpp"
#include "HidePlayerController.hpp"

#include <stdlib.h>
#include <assert.h>

const std::string GameEngineState::ARG_IP_ADDRESS = "ARG_IP_ADDRESS";
const std::string GameEngineState::ARG_USERNAME = "ARG_USERNAME";

void cb_client_onEntityRegistered(Entity* e)
{
    ENGINE_STATE_GAME.getWorld().addNetworkEntity(e);
    
    if (e->controller()->getRTTI().isExactly(HidePlayerController::rtti))
    {
        HidePlayerController* c = static_cast<HidePlayerController*>(e->controller());
        uint32_t key = c->getEntityLayoutKey();
        EntityLayoutDef& eld = ENTITY_LAYOUT_MGR.findEntityLayoutDef(key);
        ENTITY_LAYOUT_MGR.loadEntityLayout(eld, INST_REG.get<EntityIDManager>(INSK_EID_CLNT));
        ENGINE_STATE_GAME.getWorld().populateFromEntityLayout(eld, false);
    }
}

void cb_client_onEntityDeregistered(Entity* e)
{
    ENGINE_STATE_GAME.getWorld().removeNetworkEntity(e);
}

void cb_client_handleInputStateRelease(InputState* inputState)
{
    GameInputState* gis = static_cast<GameInputState*>(inputState);
    INPUT_GAME.free(gis);
}

void cb_client_removeProcessedMoves(float inLastMoveProcessedOnServerTimestamp)
{
    INPUT_GAME.getMoveList().removeProcessedMoves(inLastMoveProcessedOnServerTimestamp, cb_client_handleInputStateRelease);
}

MoveList& cb_client_getMoveList()
{
    return INPUT_GAME.getMoveList();
}

void cb_client_onPlayerWelcomed(uint8_t playerID)
{
    INPUT_GAME.inputState()->activateNextPlayer(playerID);
}

#define GAME_ENGINE_CBS cb_client_onEntityRegistered, cb_client_onEntityDeregistered, cb_client_removeProcessedMoves, cb_client_getMoveList, cb_client_onPlayerWelcomed

void GameEngineState::enter(Engine* e)
{
    createDeviceDependentResources();
    onWindowSizeChanged(e->screenWidth(), e->screenHeight(), e->cursorWidth(), e->cursorHeight());
    
    std::string serverIPAddress;
    uint16_t port;
    if (isHost())
    {
        Server::create();
        
        if (!NW_MGR_SRVR->isConnected())
        {
            e->revertToPreviousState();
            return;
        }
        
        serverIPAddress = StringUtil::format("%s:%d", "localhost", CFG_MAIN._serverPort);
        port = CFG_MAIN._clientPortHost;
    }
    else
    {
        serverIPAddress = StringUtil::format("%s:%d", _args.getString(ARG_IP_ADDRESS).c_str(), CFG_MAIN._serverPort);
        port = CFG_MAIN._clientPortJoin;
    }
    
    NetworkManagerClient::create(serverIPAddress, _args.getString(ARG_USERNAME), port, GAME_ENGINE_CBS);
    assert(NW_MGR_CLNT != NULL);
    
    if (!NW_MGR_CLNT->isConnected())
    {
        e->revertToPreviousState();
        return;
    }
    
    GOW_AUDIO.playMusic(true, 0.1f);
}

void GameEngineState::execute(Engine* e)
{
    switch (e->requestedStateAction())
    {
        case ERSA_CREATE_RESOURCES:
            createDeviceDependentResources();
            break;
        case ERSA_WINDOW_SIZE_CHANGED:
            onWindowSizeChanged(e->screenWidth(), e->screenHeight(), e->cursorWidth(), e->cursorHeight());
            break;
        case ERSA_RELEASE_RESOURCES:
            releaseDeviceDependentResources();
            break;
        case ERSA_RESUME:
            resume();
            break;
        case ERSA_PAUSE:
            pause();
            break;
        case ERSA_UPDATE:
            update(e);
            break;
        case ERSA_RENDER:
            render();
            break;
        case ERSA_DEFAULT:
        default:
            break;
    }
}

void GameEngineState::exit(Engine* e)
{
    releaseDeviceDependentResources();
    
    _world.clearLayout();
    _world.clearNetwork();
    INPUT_GAME.reset();
    
    NetworkManagerClient::destroy();
    
    if (isHost())
    {
        Server::destroy();
    }
}

Entity* GameEngineState::getControlledPlayer()
{
    uint8_t playerID = INPUT_GAME.inputState()->getPlayerInputState(0).playerID();
    Entity* ret = NULL;
    
    for (Entity* e : ENGINE_STATE_GAME._world.getPlayers())
    {
        PlayerController* c = static_cast<PlayerController*>(e->controller());
        
        if (playerID == c->getPlayerID())
        {
            ret = e;
            break;
        }
    }
    
    return ret;
}

World& GameEngineState::getWorld()
{
    return _world;
}

bool GameEngineState::isHost()
{
    return !_args.hasValue(ARG_IP_ADDRESS);
}

void GameEngineState::createDeviceDependentResources()
{
    ENTITY_MGR.initWithJSONFile("assets/json/entities.json");
    ENTITY_LAYOUT_MGR.initWithJSONFile("assets/json/layouts.json");
    ASSETS.initWithJSONFile("assets/json/assets_game.json");
    _renderer.createDeviceDependentResources();
    GOW_AUDIO.createDeviceDependentResources();
    GOW_AUDIO.setSoundsDisabled(CFG_MAIN._sfxDisabled);
    GOW_AUDIO.setMusicDisabled(CFG_MAIN._musicDisabled);
}

void GameEngineState::onWindowSizeChanged(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight)
{
    _renderer.onWindowSizeChanged(screenWidth, screenHeight);
}

void GameEngineState::releaseDeviceDependentResources()
{
    ENTITY_MGR.clear();
    ENTITY_LAYOUT_MGR.clear();
    ASSETS.clear();
    _renderer.releaseDeviceDependentResources();
    GOW_AUDIO.releaseDeviceDependentResources();
}

void GameEngineState::resume()
{
    GOW_AUDIO.resume();
}

void GameEngineState::pause()
{
    GOW_AUDIO.pause();
}

void GameEngineState::update(Engine* e)
{
    INST_REG.get<TimeTracker>(INSK_TIME_CLNT)->onFrame();
    
    NW_MGR_CLNT->processIncomingPackets();
    if (NW_MGR_CLNT->state() == NWCS_DISCONNECTED)
    {
        e->revertToPreviousState();
        return;
    }
    
    if (NW_MGR_CLNT->hasReceivedNewState())
    {
        for (Entity* e : _world.getPlayers())
        {
            e->networkController()->recallCache();
        }
        
        for (Entity* e : _world.getNetworkEntities())
        {
            e->networkController()->recallCache();
        }
        
        for (const Move& move : INPUT_GAME.getMoveList())
        {
            updateWorld(&move, false);
        }
    }
    
    GameInputManagerState gims = INPUT_GAME.update();
    if (gims == GIMS_EXIT)
    {
        e->revertToPreviousState();
        return;
    }
    
    updateWorld(INPUT_GAME.getPendingMove(), true);
    
    NW_MGR_CLNT->sendOutgoingPackets();
    
    if (isHost())
    {
        Server::getInstance()->update();
    }
}

void GameEngineState::render()
{
    _renderer.render();
    GOW_AUDIO.render();
}

void GameEngineState::updateWorld(const Move* move, bool isLocal)
{
    assert(move != NULL);
    
    for (Entity* e : _world.getPlayers())
    {
        PlayerController* c = static_cast<PlayerController*>(e->controller());
        c->processInput(move->inputState(), isLocal);
    }
    
    _world.stepPhysics(INST_REG.get<TimeTracker>(INSK_TIME_CLNT));
    
    for (Entity* e : _world.getPlayers())
    {
        e->update();
    }
    
    for (Entity* e : _world.getNetworkEntities())
    {
        e->update();
    }
}

GameEngineState::GameEngineState() : State<Engine>(),
_world(),
_renderer()
{
    // Empty
}
