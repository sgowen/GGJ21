//
//  GameInputManager.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <GowEngine/GowEngine.hpp>

#include <string>

enum GameInputManagerState
{
    GIMS_DEFAULT,
    GIMS_EXIT
};

enum InputStateFlags
{
    GISF_MOVING_RIGHT = 1 << 0,
    GISF_MOVING_LEFT =  1 << 1,
    GISF_MOVING_UP =    1 << 2,
    GISF_MOVING_DOWN =  1 << 3,
    GISF_CONFIRM     =  1 << 4,
    GISF_CANCEL      =  1 << 5
};

#define INPUT_GAME GameInputManager::getInstance()

class GameInputManager
{
public:
    static GameInputManager& getInstance()
    {
        static GameInputManager ret = GameInputManager();
        return ret;
    }
    
    static void cb_inputStateRelease(InputState* is)
    {
        INPUT_GAME.free(is);
    }
    
    void setMatrix(Matrix* m);
    GameInputManagerState update();
    const Move& sampleInputAsNewMove();
    InputState* inputState();
    MoveList& moveList();
    void free(InputState* gis);
    void reset();
    
private:
    GameInputManagerState _state;
    
    Pool<InputState> _poolInputState;
    InputState* _inputState;
    MoveList _moveList;
    Matrix* _matrix;
    
    void drop2ndPlayer();
    
    GameInputManager();
    ~GameInputManager();
    GameInputManager(const GameInputManager&);
    GameInputManager& operator=(const GameInputManager&);
};
