//
//  GameInputManager.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GameInputManager.hpp"

#include "InputManager.hpp"
#include "KeyboardLookup.hpp"
#include "Timing.hpp"
#include "InstanceManager.hpp"
#include "Network.hpp"
#include "NetworkManagerClient.hpp"
#include "Macros.hpp"
#include "MathUtil.hpp"
#include "MainConfig.hpp"

GameInputManager& GameInputManager::getInstance()
{
    static GameInputManager ret = GameInputManager();
    return ret;
}

void GameInputManager::sRemoveProcessedMoves(float inLastMoveProcessedOnServerTimestamp)
{
    getInstance().getMoveList().removeProcessedMoves(inLastMoveProcessedOnServerTimestamp, GameInputManager::sHandleInputStateRelease);
}

MoveList& GameInputManager::sGetMoveList()
{
    return getInstance().getMoveList();
}

void GameInputManager::sOnPlayerWelcomed(uint8_t playerID)
{
    getInstance()._currentState->activateNextPlayer(playerID);
}

void GameInputManager::sHandleInputStateRelease(InputState* inputState)
{
    GameInputState* gameInputState = static_cast<GameInputState*>(inputState);
    getInstance()._inputStates.free(gameInputState);
}

GameInputManagerState GameInputManager::update()
{
    _state = GIMS_DEFAULT;
    
#if IS_MOBILE
    for (CursorEvent* e : INPUT_MGR.getCursorEvents())
    {
        Vector2& v = INPUT_MGR.convert(e);
        SET_BIT(_currentState->getPlayerInputState(0)._inputState, GISF_MOVING_UP, e->isPressed() && v.y() > (CFG_MAIN._camHeight / 2));
        SET_BIT(_currentState->getPlayerInputState(0)._inputState, GISF_MOVING_LEFT, e->isPressed() && v.x() < (CFG_MAIN._camWidth / 2));
        SET_BIT(_currentState->getPlayerInputState(0)._inputState, GISF_MOVING_DOWN, e->isPressed() && v.y() < (CFG_MAIN._camHeight / 2));
        SET_BIT(_currentState->getPlayerInputState(0)._inputState, GISF_MOVING_RIGHT, e->isPressed() && v.x() > (CFG_MAIN._camHeight / 2));
    }
#endif
    
    bool isMovingUp[4] = {false};
    bool isMovingLeft[4] = {false};
    bool isMovingDown[4] = {false};
    bool isMovingRight[4] = {false};
    for (GamepadEvent* e : INPUT_MGR.getGamepadEvents())
    {
        switch (e->_type)
        {
            case GPET_BUTTON_SELECT:
                _state = e->isPressed() ? GIMS_EXIT : _state;
                continue;
            case GPET_D_PAD_UP:
            {
                if (!isMovingUp[e->_index])
                {
                    isMovingUp[e->_index] = e->isPressed();
                    SET_BIT(_currentState->getPlayerInputState(e->_index)._inputState, GISF_MOVING_UP, isMovingUp[e->_index]);
                }
                continue;
            }
            case GPET_D_PAD_LEFT:
            {
                if (!isMovingLeft[e->_index])
                {
                    isMovingLeft[e->_index] = e->isPressed();
                    SET_BIT(_currentState->getPlayerInputState(e->_index)._inputState, GISF_MOVING_LEFT, isMovingLeft[e->_index]);
                }
                continue;
            }
            case GPET_D_PAD_DOWN:
            {
                if (!isMovingDown[e->_index])
                {
                    isMovingDown[e->_index] = e->isPressed();
                    SET_BIT(_currentState->getPlayerInputState(e->_index)._inputState, GISF_MOVING_DOWN, isMovingDown[e->_index]);
                }
                continue;
            }
            case GPET_D_PAD_RIGHT:
            {
                if (!isMovingRight[e->_index])
                {
                    isMovingRight[e->_index] = e->isPressed();
                    SET_BIT(_currentState->getPlayerInputState(e->_index)._inputState, GISF_MOVING_RIGHT, isMovingRight[e->_index]);
                }
                continue;
            }
            case GPET_STICK_LEFT:
            {
                float xVal = sanitizeCloseToZeroValue(e->_x);
                float yVal = sanitizeCloseToZeroValue(e->_y);
                
                if (!isMovingUp[e->_index])
                {
                    isMovingUp[e->_index] = yVal < 0;
                    SET_BIT(_currentState->getPlayerInputState(e->_index)._inputState, GISF_MOVING_UP, isMovingUp[e->_index]);
                }
                
                if (!isMovingLeft[e->_index])
                {
                    isMovingLeft[e->_index] = xVal < 0;
                    SET_BIT(_currentState->getPlayerInputState(e->_index)._inputState, GISF_MOVING_LEFT, isMovingLeft[e->_index]);
                }
                
                if (!isMovingDown[e->_index])
                {
                    isMovingDown[e->_index] = yVal > 0;
                    SET_BIT(_currentState->getPlayerInputState(e->_index)._inputState, GISF_MOVING_DOWN, isMovingDown[e->_index]);
                }
                
                if (!isMovingRight[e->_index])
                {
                    isMovingRight[e->_index] = xVal > 0;
                    SET_BIT(_currentState->getPlayerInputState(e->_index)._inputState, GISF_MOVING_RIGHT, isMovingRight[e->_index]);
                }
                continue;
            }
            default:
                continue;
        }
    }
    
    for (KeyboardEvent* e : INPUT_MGR.getKeyboardEvents())
    {
        switch (e->_key)
        {
            case GOW_KEY_ESCAPE:
                _state = GIMS_EXIT;
                continue;
            case GOW_KEY_W: // Player 1
                SET_BIT(_currentState->getPlayerInputState(0)._inputState, GISF_MOVING_UP, e->isPressed());
                continue;
            case GOW_KEY_A: // Player 1
                SET_BIT(_currentState->getPlayerInputState(0)._inputState, GISF_MOVING_LEFT, e->isPressed());
                continue;
            case GOW_KEY_S: // Player 1
                SET_BIT(_currentState->getPlayerInputState(0)._inputState, GISF_MOVING_DOWN, e->isPressed());
                continue;
            case GOW_KEY_D: // Player 1
                SET_BIT(_currentState->getPlayerInputState(0)._inputState, GISF_MOVING_RIGHT, e->isPressed());
                continue;
            case GOW_KEY_ARROW_UP: // Player 2
                SET_BIT(_currentState->getPlayerInputState(1)._inputState, GISF_MOVING_UP, e->isPressed());
                continue;
            case GOW_KEY_ARROW_LEFT: // Player 2
                SET_BIT(_currentState->getPlayerInputState(1)._inputState, GISF_MOVING_LEFT, e->isPressed());
                continue;
            case GOW_KEY_ARROW_DOWN: // Player 2
                SET_BIT(_currentState->getPlayerInputState(1)._inputState, GISF_MOVING_DOWN, e->isPressed());
                continue;
            case GOW_KEY_ARROW_RIGHT: // Player 2
                SET_BIT(_currentState->getPlayerInputState(1)._inputState, GISF_MOVING_RIGHT, e->isPressed());
                continue;
            case GOW_KEY_PERIOD:
                drop2ndPlayer();
                continue;
            default:
                continue;
        }
    }
    
    if (_currentState->isRequestingToAddLocalPlayer())
    {
        NW_MGR_CLIENT->requestToAddLocalPlayer();
    }
    
    _pendingMove = &sampleInputAsMove();
    
    return _state;
}

const Move* GameInputManager::getPendingMove()
{
    return _pendingMove;
}

void GameInputManager::clearPendingMove()
{
    _pendingMove = NULL;
}

GameInputState* GameInputManager::getInputState()
{
    return _currentState;
}

MoveList& GameInputManager::getMoveList()
{
    return _moveList;
}

const Move& GameInputManager::sampleInputAsMove()
{
    GameInputState* inputState = _inputStates.obtain();
    _currentState->copyTo(inputState);
    
    Timing* t = static_cast<Timing*>(INSTANCE_MGR.get(INSK_TIMING_CLIENT));
    
    return _moveList.addMove(inputState, t->getTime());
}

void GameInputManager::drop2ndPlayer()
{
    _currentState->getPlayerInputState(1)._playerID = NW_INPUT_UNASSIGNED;
    NW_MGR_CLIENT->requestToDropLocalPlayer(1);
}

GameInputManager::GameInputManager() :
_currentState(_inputStates.obtain()),
_pendingMove(NULL),
_state(GIMS_DEFAULT)
{
    // Empty
}

GameInputManager::~GameInputManager()
{
    _inputStates.free(_currentState);
}
