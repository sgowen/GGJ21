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

#define INPUT_MANAGER_CALLBACKS GameInputManager::sRemoveProcessedMoves, GameInputManager::sGetMoveList, GameInputManager::sOnPlayerWelcomed

enum GameInputManagerState
{
    GIMS_DEFAULT,
    GIMS_EXIT
};

#define INPUT_GAME GameInputManager::getInstance()

class GameInputManager
{
public:
    static GameInputManager& getInstance();
    
    static void sRemoveProcessedMoves(float lastMoveProcessedOnServerTimestamp);
    static MoveList& sGetMoveList();
    static void sOnPlayerWelcomed(uint8_t playerID);
    static void sHandleInputStateRelease(InputState* inputState);
    
    GameInputManagerState update();
    const Move* getPendingMove();
    void clearPendingMove();
    GameInputState* getInputState();
    MoveList& getMoveList();
    void reset();
    
private:
    GameInputManagerState _state;
    
    Pool<GameInputState> _inputStates;
    GameInputState* _currentState;
    MoveList _moveList;
    const Move* _pendingMove;
    
    const Move& sampleInputAsMove();
    void drop2ndPlayer();
    
    GameInputManager();
    ~GameInputManager();
    GameInputManager(const GameInputManager&);
    GameInputManager& operator=(const GameInputManager&);
};
