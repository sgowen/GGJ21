//
//  GameInputManager.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GameInputManager.hpp"

#include "InputManager.hpp"
#include "TimeTracker.hpp"
#include "InstanceRegistry.hpp"
#include "Network.hpp"
#include "NetworkManagerClient.hpp"
#include "Macros.hpp"
#include "MathUtil.hpp"
#include "MainConfig.hpp"
#include "GameEngineState.hpp"
#include "PlayerController.hpp"
#include "World.hpp"
#include "Entity.hpp"

GameInputManagerState GameInputManager::update()
{
    _state = GIMS_DEFAULT;
    
    Entity* controlledPlayer = ENGINE_STATE_GAME.getControlledPlayer();
    if (controlledPlayer != NULL)
    {
        float playerX = controlledPlayer == NULL ? 0 : controlledPlayer->getPosition()._x;
        float playerY = controlledPlayer == NULL ? 0 : controlledPlayer->getPosition()._y;
        for (CursorEvent* e : INPUT_MGR.getCursorEvents())
        {
            Vector2& v = INPUT_MGR.convert(e);
            SET_BIT(_inputState->getPlayerInputState(0)._inputState, GISF_MOVING_UP, e->isPressed() && v._y > playerY + 4);
            SET_BIT(_inputState->getPlayerInputState(0)._inputState, GISF_MOVING_LEFT, e->isPressed() && v._x < playerX - 4);
            SET_BIT(_inputState->getPlayerInputState(0)._inputState, GISF_MOVING_DOWN, e->isPressed() && v._y < playerY - 4);
            SET_BIT(_inputState->getPlayerInputState(0)._inputState, GISF_MOVING_RIGHT, e->isPressed() && v._x > playerX + 4);
        }
    }
    
    bool isMovingUp[4] = {false};
    bool isMovingLeft[4] = {false};
    bool isMovingDown[4] = {false};
    bool isMovingRight[4] = {false};
    for (GamepadEvent* e : INPUT_MGR.getGamepadEvents())
    {
        switch (e->_button)
        {
            case GPEB_BUTTON_SELECT:
                _state = e->isDown() ? GIMS_EXIT : _state;
                continue;
            case GPEB_BUMPER_LEFT:
            case GPEB_BUMPER_RIGHT:
            {
                if (e->isDown())
                {
                    drop2ndPlayer();
                }
                continue;
            }
            case GPEB_D_PAD_UP:
            {
                if (!isMovingUp[e->_index])
                {
                    isMovingUp[e->_index] = e->isPressed();
                    SET_BIT(_inputState->getPlayerInputState(e->_index)._inputState, GISF_MOVING_UP, isMovingUp[e->_index]);
                }
                continue;
            }
            case GPEB_D_PAD_LEFT:
            {
                if (!isMovingLeft[e->_index])
                {
                    isMovingLeft[e->_index] = e->isPressed();
                    SET_BIT(_inputState->getPlayerInputState(e->_index)._inputState, GISF_MOVING_LEFT, isMovingLeft[e->_index]);
                }
                continue;
            }
            case GPEB_D_PAD_DOWN:
            {
                if (!isMovingDown[e->_index])
                {
                    isMovingDown[e->_index] = e->isPressed();
                    SET_BIT(_inputState->getPlayerInputState(e->_index)._inputState, GISF_MOVING_DOWN, isMovingDown[e->_index]);
                }
                continue;
            }
            case GPEB_D_PAD_RIGHT:
            {
                if (!isMovingRight[e->_index])
                {
                    isMovingRight[e->_index] = e->isPressed();
                    SET_BIT(_inputState->getPlayerInputState(e->_index)._inputState, GISF_MOVING_RIGHT, isMovingRight[e->_index]);
                }
                continue;
            }
            case GPEB_STICK_LEFT:
            {
                if (!isMovingUp[e->_index])
                {
                    isMovingUp[e->_index] = e->_y < 0;
                    SET_BIT(_inputState->getPlayerInputState(e->_index)._inputState, GISF_MOVING_UP, isMovingUp[e->_index]);
                }
                
                if (!isMovingLeft[e->_index])
                {
                    isMovingLeft[e->_index] = e->_x < 0;
                    SET_BIT(_inputState->getPlayerInputState(e->_index)._inputState, GISF_MOVING_LEFT, isMovingLeft[e->_index]);
                }
                
                if (!isMovingDown[e->_index])
                {
                    isMovingDown[e->_index] = e->_y > 0;
                    SET_BIT(_inputState->getPlayerInputState(e->_index)._inputState, GISF_MOVING_DOWN, isMovingDown[e->_index]);
                }
                
                if (!isMovingRight[e->_index])
                {
                    isMovingRight[e->_index] = e->_x > 0;
                    SET_BIT(_inputState->getPlayerInputState(e->_index)._inputState, GISF_MOVING_RIGHT, isMovingRight[e->_index]);
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
                SET_BIT(_inputState->getPlayerInputState(0)._inputState, GISF_MOVING_UP, e->isPressed());
                continue;
            case GOW_KEY_A: // Player 1
                SET_BIT(_inputState->getPlayerInputState(0)._inputState, GISF_MOVING_LEFT, e->isPressed());
                continue;
            case GOW_KEY_S: // Player 1
                SET_BIT(_inputState->getPlayerInputState(0)._inputState, GISF_MOVING_DOWN, e->isPressed());
                continue;
            case GOW_KEY_D: // Player 1
                SET_BIT(_inputState->getPlayerInputState(0)._inputState, GISF_MOVING_RIGHT, e->isPressed());
                continue;
            case GOW_KEY_ARROW_UP: // Player 2
                SET_BIT(_inputState->getPlayerInputState(1)._inputState, GISF_MOVING_UP, e->isPressed());
                continue;
            case GOW_KEY_ARROW_LEFT: // Player 2
                SET_BIT(_inputState->getPlayerInputState(1)._inputState, GISF_MOVING_LEFT, e->isPressed());
                continue;
            case GOW_KEY_ARROW_DOWN: // Player 2
                SET_BIT(_inputState->getPlayerInputState(1)._inputState, GISF_MOVING_DOWN, e->isPressed());
                continue;
            case GOW_KEY_ARROW_RIGHT: // Player 2
                SET_BIT(_inputState->getPlayerInputState(1)._inputState, GISF_MOVING_RIGHT, e->isPressed());
                continue;
            case GOW_KEY_PERIOD:
            {
                if (e->isDown())
                {
                    drop2ndPlayer();
                }
                continue;
            }
            default:
                continue;
        }
    }
    
    if (_inputState->isRequestingToAddLocalPlayer())
    {
        NW_MGR_CLNT->requestToAddLocalPlayer();
    }
    
    return _state;
}

const Move& GameInputManager::sampleInputAsNewMove()
{
    GameInputState* inputState = _poolGameInputState.obtain();
    _inputState->copyTo(inputState);
    
    TimeTracker* tt = INST_REG.get<TimeTracker>(INSK_TIME_CLNT);
    return _moveList.addMove(inputState, tt->_time, NW_MGR_CLNT->getNumMovesProcessed());
}

GameInputState* GameInputManager::inputState()
{
    return _inputState;
}

MoveList& GameInputManager::moveList()
{
    return _moveList;
}

void GameInputManager::free(GameInputState *gis)
{
    if (gis == NULL)
    {
        return;
    }
    
    _poolGameInputState.free(gis);
}

void GameInputManager::reset()
{
    free(_inputState);
    
    _inputState = _poolGameInputState.obtain();
    _inputState->reset();
    
    _state = GIMS_DEFAULT;
    _moveList.clear();
}

void GameInputManager::drop2ndPlayer()
{
    GameInputState::PlayerInputState& pis = _inputState->getPlayerInputState(1);
    pis._playerID = NW_INPUT_UNASSIGNED;
    NW_MGR_CLNT->requestToDropLocalPlayer(1);
}

GameInputManager::GameInputManager() :
_inputState(NULL),
_state(GIMS_DEFAULT)
{
    reset();
}

GameInputManager::~GameInputManager()
{
    free(_inputState);
}
