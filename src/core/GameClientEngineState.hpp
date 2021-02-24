//
//  GameClientEngineState.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "StateMachine.hpp"
#include "World.hpp"
#include "Renderer.hpp"

class Engine;
class TimeTracker;
class Server;
class Move;
class Entity;

#define ENGINE_STATE_GAME_CLNT GameClientEngineState::getInstance()

class GameClientEngineState : public State<Engine>
{
    friend class GameInputManager;
    friend class GameRenderer;
    
public:    
    static GameClientEngineState& getInstance()
    {
        static GameClientEngineState ret = GameClientEngineState();
        return ret;
    }
    
    virtual void enter(Engine* e);
    virtual void execute(Engine* e);
    virtual void exit(Engine* e);
    
    Entity* getControlledPlayer();
    World& getWorld();
    
private:
    World _world;
    Renderer _renderer;
    
    void createDeviceDependentResources();
    void onWindowSizeChanged(uint16_t screenWidth, uint16_t screenHeight);
    void releaseDeviceDependentResources();
    void resume();
    void pause();
    void update(Engine* e);
    void render();
    
    void updateWorld(const Move& move, bool isLive);
    
    GameClientEngineState();
    virtual ~GameClientEngineState() {}
    GameClientEngineState(const GameClientEngineState&);
    GameClientEngineState& operator=(const GameClientEngineState&);
};
