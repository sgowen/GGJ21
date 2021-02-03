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
#include "EntityManager.hpp"
#include "SocketClientHelper.hpp"
#include "GowAudioEngine.hpp"
#include "Assets.hpp"
#include "EntityMapper.hpp"
#include "EntityLayoutMapper.hpp"
#include "MainConfig.hpp"
#include "PlayerController.hpp"
#include "InstanceManager.hpp"
#include "Config.hpp"
#include "Macros.hpp"
#include "Network.hpp"

#include <stdlib.h>
#include <assert.h>

uint64_t GameEngineState::sGetPlayerAddressHash(uint8_t playerIndex)
{
    uint64_t ret = 0;
    
    World& w = ENGINE_STATE_GAME._world;
    
    uint8_t playerID = playerIndex + 1;
    
    PlayerController* player = NULL;
    for (Entity* e : w.getPlayers())
    {
        PlayerController* pc = static_cast<PlayerController*>(e->getController());
        if (pc->getPlayerID() == playerID)
        {
            player = pc;
            break;
        }
    }
    
    if (player != NULL)
    {
        ret = player->getAddressHash();
    }
    
    return ret;
}

void GameEngineState::sHandleDynamicEntityCreatedOnClient(Entity* e)
{
    ENGINE_STATE_GAME._world.addEntity(e);
}

void GameEngineState::sHandleDynamicEntityDeletedOnClient(Entity* e)
{
    ENGINE_STATE_GAME._world.removeEntity(e);
}

void GameEngineState::sHandleHostServer(Engine* e, std::string name)
{
    Server::create();
    
    ENGINE_STATE_GAME._name = name;
    ENGINE_STATE_GAME._isHost = true;
    SET_BIT(ENGINE_STATE_GAME._state, GESS_HOST, true);
    SET_BIT(ENGINE_STATE_GAME._state, GESS_CONNECTED, false);
    
    e->getStateMachine().changeState(&ENGINE_STATE_GAME);
}

void GameEngineState::sHandleJoinServer(Engine* e, std::string serverIPAddress, std::string name)
{
    ENGINE_STATE_GAME._serverIPAddress = serverIPAddress;
    ENGINE_STATE_GAME._name = name;
    ENGINE_STATE_GAME._isHost = false;
    ENGINE_STATE_GAME.joinServer();
    
    e->getStateMachine().changeState(&ENGINE_STATE_GAME);
}

void GameEngineState::enter(Engine* e)
{
    createDeviceDependentResources();
    onWindowSizeChanged(e->screenWidth(), e->screenHeight(), e->cursorWidth(), e->cursorHeight());
}

void GameEngineState::execute(Engine* e)
{
    switch (e->state())
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
    
    _world.clear();
    _timeTracker->reset();
    INPUT_GAME.reset();
    _map = 0;
    
    if (NW_MGR_CLIENT != NULL)
    {
        NetworkManagerClient::destroy();
    }
    
    if (Server::getInstance() != NULL)
    {
        Server::destroy();
    }
    
    _state = GESS_DEFAULT;
    _serverIPAddress.clear();
    _name.clear();
    _isHost = false;
    _isLive = false;
}

World& GameEngineState::getWorld()
{
    return _world;
}

bool GameEngineState::isLive()
{
    return _isLive;
}

#define GAME_ENGINE_CALLBACKS GameEngineState::sHandleDynamicEntityCreatedOnClient, GameEngineState::sHandleDynamicEntityDeletedOnClient

void GameEngineState::joinServer()
{
    uint16_t port = _isHost ? CFG_MAIN._clientPortHost : CFG_MAIN._clientPortJoin;
    NetworkManagerClient::create(new SocketClientHelper(_serverIPAddress, _name, port, NW_MGR_CLIENT_CALLBACKS), GAME_ENGINE_CALLBACKS, INPUT_MANAGER_CALLBACKS);
    
    assert(NW_MGR_CLIENT != NULL);
    
    SET_BIT(_state, GESS_CONNECTED, true);
}

void GameEngineState::updateWorld(const Move* move)
{
    assert(move != NULL);
    
    for (Entity* e : _world.getPlayers())
    {
        PlayerController* pc = static_cast<PlayerController*>(e->getController());
        pc->processInput(move->getInputState());
    }
    
    _world.stepPhysics();
    
    for (Entity* e : _world.getDynamicEntities())
    {
        e->update();
    }
}

void GameEngineState::createDeviceDependentResources()
{
    ENTITY_MAPPER.initWithJSONFile("json/entities.json");
    ENTITY_LAYOUT_MAPPER.initWithJSONFile("json/maps.json");
    ASSETS.initWithJSONFile("json/assets_game.json");
    _renderer.createDeviceDependentResources();
    GOW_AUDIO.createDeviceDependentResources();
    // TODO, remove
    GOW_AUDIO.playSound(_isHost ? 115 : 59);
    GOW_AUDIO.playMusic(true, 0.1f);
}

void GameEngineState::onWindowSizeChanged(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight)
{
    _renderer.onWindowSizeChanged(screenWidth, screenHeight);
}

void GameEngineState::releaseDeviceDependentResources()
{
    ENTITY_MAPPER.clear();
    ENTITY_LAYOUT_MAPPER.clear();
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
    if (IS_BIT_SET(_state, GESS_HOST) &&
        !IS_BIT_SET(_state, GESS_CONNECTED))
    {
        if (NW_MGR_SERVER != NULL &&
            NW_MGR_SERVER->isConnected())
        {
            _serverIPAddress = std::string("localhost:9999");
            
            joinServer();
        }
        
        return;
    }
    
    _timeTracker->onFrame();
    
    NW_MGR_CLIENT->processIncomingPackets();
    if (NW_MGR_CLIENT->state() == NWCS_DISCONNECTED)
    {
        e->getStateMachine().revertToPreviousState();
        return;
    }
    
    if (NW_MGR_CLIENT->hasReceivedNewState())
    {
        uint32_t map = NW_MGR_CLIENT->getMap();
        if (map != 0 && _map != map)
        {
            _map = map;
            _world.loadMap(_map);
        }
        
        for (Entity* e : _world.getDynamicEntities())
        {
            e->getNetworkController()->recallNetworkCache();
        }
        
        // all processed moves have been removed, so all that are left are unprocessed moves so we must apply them...
        for (const Move& move : INPUT_GAME.getMoveList())
        {
            updateWorld(&move);
        }
    }
    
    GameInputManagerState gims = INPUT_GAME.update();
    if (gims == GIMS_EXIT)
    {
        e->getStateMachine().revertToPreviousState();
        return;
    }
    
    _isLive = true;
    updateWorld(INPUT_GAME.getPendingMove());
    _isLive = false;
    INPUT_GAME.clearPendingMove();
    
    NW_MGR_CLIENT->sendOutgoingPackets();
    
    if (_isHost)
    {
        Server::getInstance()->update();
    }
}

void GameEngineState::render()
{
    _renderer.render();
    GOW_AUDIO.render();
}

GameEngineState::GameEngineState() : State<Engine>(),
_renderer(),
_timeTracker(INSTANCE_MGR.get<TimeTracker>(INSK_TIMING_CLIENT)),
_world(_timeTracker, INSTANCE_MGR.get<EntityIDManager>(INSK_ENTITY_ID_MANAGER_CLIENT), WorldFlag_Client),
_state(GESS_DEFAULT),
_serverIPAddress(""),
_name(""),
_map(0),
_isHost(false),
_isLive(false)
{
    // Empty
}
