//
//  GameEngineState.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "StateMachine.hpp"

#include "GameRenderer.hpp"
#include "World.hpp"

enum GameEngineStateState
{
    GESS_DEFAULT =       0,
    GESS_HOST =          1 << 0,
    GESS_CONNECTED =     1 << 1
};

class Engine;
class World;
class TimeTracker;
class Server;
class Move;

#define ENGINE_STATE_GAME GameEngineState::getInstance()

class GameEngineState : public State<Engine>
{
    friend class GameInputManager;
    friend class GameRenderer;
    
public:
    static GameEngineState& getInstance()
    {
        static GameEngineState ret = GameEngineState();
        return ret;
    }
    
    static uint64_t sGetPlayerAddressHash(uint8_t playerIndex);
    static void sHandleDynamicEntityCreatedOnClient(Entity* e);
    static void sHandleDynamicEntityDeletedOnClient(Entity* e);
    static void sHandleHostServer(Engine* e, std::string name);
    static void sHandleJoinServer(Engine* e, std::string serverIPAddress, std::string name);
    
    virtual void enter(Engine* e);
    virtual void execute(Engine* e);
    virtual void exit(Engine* e);
    
    World& getWorld();
    bool isLive();
    
private:    
    GameRenderer _renderer;
    TimeTracker* _timeTracker;
    World _world;
    uint32_t _state;
    std::string _serverIPAddress;
    std::string _name;
    uint32_t _map;
    bool _isHost;
    bool _isLive;
    
    void joinServer();
    void updateWorld(const Move* move);
    
    void createDeviceDependentResources();
    void onWindowSizeChanged(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight);
    void releaseDeviceDependentResources();
    void resume();
    void pause();
    void update(Engine* e);
    void render();
    
    GameEngineState();
    ~GameEngineState() {}
    GameEngineState(const GameEngineState&);
    GameEngineState& operator=(const GameEngineState&);
};
