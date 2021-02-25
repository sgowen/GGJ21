//
//  GameClientEngineState.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "EngineState.hpp"
#include "World.hpp"

class Engine;
class TimeTracker;
class Server;
class Move;
class Entity;

#define ENGINE_STATE_GAME_CLNT GameClientEngineState::getInstance()

class GameClientEngineState : public EngineState
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
    virtual void exit(Engine* e);
    virtual void update(Engine* e);
    virtual void render(Engine* e);
    
    Entity* getControlledPlayer();
    World& getWorld();
    
private:
    World _world;
    
    void updateWorld(const Move& move, bool isLive);
    
    GameClientEngineState();
    virtual ~GameClientEngineState() {}
    GameClientEngineState(const GameClientEngineState&);
    GameClientEngineState& operator=(const GameClientEngineState&);
};
