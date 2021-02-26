//
//  GameInputManager.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "MoveList.hpp"
#include "Pool.hpp"
#include "GameInputState.hpp"

#include <string>

enum GameInputManagerState
{
    GIMS_DEFAULT,
    GIMS_EXIT
};

#define INPUT_GAME GameInputManager::getInstance()

class Matrix;

class GameInputManager
{
public:
    static GameInputManager& getInstance()
    {
        static GameInputManager ret = GameInputManager();
        return ret;
    }
    
    void setMatrix(Matrix* m);
    GameInputManagerState update();
    const Move& sampleInputAsNewMove();
    GameInputState* inputState();
    MoveList& moveList();
    void free(GameInputState* gis);
    void reset();
    
private:
    GameInputManagerState _state;
    
    Pool<GameInputState> _poolGameInputState;
    GameInputState* _inputState;
    MoveList _moveList;
    Matrix* _matrix;
    
    void drop2ndPlayer();
    
    GameInputManager();
    ~GameInputManager();
    GameInputManager(const GameInputManager&);
    GameInputManager& operator=(const GameInputManager&);
};
