//
//  GameEngineState.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "StateMachine.hpp"
#include "World.hpp"
#include "GameRenderer.hpp"

class Engine;
class TimeTracker;
class Server;
class Move;
class Entity;

#define ENGINE_STATE_GAME GameEngineState::getInstance()

class GameEngineState : public State<Engine>
{
    friend class GameInputManager;
    friend class GameRenderer;
    
public:
    static const std::string ARG_IP_ADDRESS;
    static const std::string ARG_USERNAME;
    
    static GameEngineState& getInstance()
    {
        static GameEngineState ret = GameEngineState();
        return ret;
    }
    
    virtual void enter(Engine* e);
    virtual void execute(Engine* e);
    virtual void exit(Engine* e);
    
    Entity* getControlledPlayer();
    World& getWorld();
    bool isHost();
    
private:
    World _world;
    GameRenderer _renderer;
    
    void createDeviceDependentResources();
    void onWindowSizeChanged(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight);
    void releaseDeviceDependentResources();
    void resume();
    void pause();
    void update(Engine* e);
    void render();
    
    void updateWorld(const Move* move, bool isLocal);
    
    GameEngineState();
    ~GameEngineState() {}
    GameEngineState(const GameEngineState&);
    GameEngineState& operator=(const GameEngineState&);
};
