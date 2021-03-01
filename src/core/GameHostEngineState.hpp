//
//  GameHostEngineState.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <GowEngine/GowEngine.hpp>

#define ENGINE_STATE_GAME_HOST GameHostEngineState::getInstance()

class GameHostEngineState : public State<Engine>
{    
public:
    static GameHostEngineState& getInstance()
    {
        static GameHostEngineState ret = GameHostEngineState();
        return ret;
    }
    
    virtual void enter(Engine* e);
    virtual void execute(Engine* e);
    virtual void exit(Engine* e);
    
private:
    GameHostEngineState() : State<Engine>() {}
    virtual ~GameHostEngineState() {}
    GameHostEngineState(const GameHostEngineState&);
    GameHostEngineState& operator=(const GameHostEngineState&);
};
