//
//  GameInputManager.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

void GameInputManager::setMatrix(Matrix* m)
{
    _matrix = m;
    INPUT_MGR.setMatrix(m);
}

GameInputManagerState GameInputManager::update()
{
    _state = GIMS_DEFAULT;
    
    uint8_t& inputStateP1 = _inputState->playerInputState(0)._inputState;
    uint8_t& inputStateP2 = _inputState->playerInputState(1)._inputState;
    
    Entity* controlledPlayer = ENGINE_STATE_GAME_CLNT.getControlledPlayer();
    if (controlledPlayer != nullptr && _matrix != nullptr)
    {
        uint8_t playerID = controlledPlayer->metadata().getUInt("playerID");
        float playerX = controlledPlayer->position()._x;
        float playerY = controlledPlayer->position()._y;
        float width = _matrix->_desc.width();
        float height = _matrix->_desc.height();
        for (CursorEvent* e : INPUT_MGR.getCursorEvents())
        {
            Vector2& v = INPUT_MGR.convert(e);
            if (playerID == 1 && v._x > (width / 2))
            {
                SET_BIT(inputStateP1, GISF_CONFIRM, e->isPressed() && v._y < height / 2);
                SET_BIT(inputStateP1, GISF_CANCEL, e->isPressed() && v._y > height / 2);
                continue;
            }
            else if (playerID == 2 && v._x < (width / 2))
            {
                SET_BIT(inputStateP1, GISF_CONFIRM, e->isPressed() && v._y < height / 2);
                SET_BIT(inputStateP1, GISF_CANCEL, e->isPressed() && v._y > height / 2);
                continue;
            }
            
            SET_BIT(inputStateP1, GISF_MOVING_UP, e->isPressed() && v._y > playerY + 4);
            SET_BIT(inputStateP1, GISF_MOVING_LEFT, e->isPressed() && v._x < playerX - 4);
            SET_BIT(inputStateP1, GISF_MOVING_DOWN, e->isPressed() && v._y < playerY - 4);
            SET_BIT(inputStateP1, GISF_MOVING_RIGHT, e->isPressed() && v._x > playerX + 4);
        }
    }
    
    for (GamepadEvent* e : INPUT_MGR.getGamepadEvents())
    {
        uint8_t& inputState = e->_index == 0 ? inputStateP1 : inputStateP2;
        
        switch (e->_button)
        {
            case GPEB_BUTTON_SELECT:
            case GPEB_BUTTON_SNES_SELECT:
                _state = e->isDown() ? GIMS_EXIT : _state;
                break;
            case GPEB_BUMPER_LEFT:
            case GPEB_BUMPER_RIGHT:
            {
                if (e->isDown())
                {
                    drop2ndPlayer();
                }
                break;
            }
            case GPEB_BUTTON_A:
                SET_BIT(inputState, GISF_CONFIRM, e->isPressed());
                break;
            case GPEB_BUTTON_B:
                SET_BIT(inputState, GISF_CANCEL, e->isPressed());
                break;
            case GPEB_D_PAD_UP:
            {
                SET_BIT(inputState, GISF_MOVING_UP, e->isPressed());
                break;
            }
            case GPEB_D_PAD_LEFT:
            {
                SET_BIT(inputState, GISF_MOVING_LEFT, e->isPressed());
                break;
            }
            case GPEB_D_PAD_DOWN:
            {
                SET_BIT(inputState, GISF_MOVING_DOWN, e->isPressed());
                break;
            }
            case GPEB_D_PAD_RIGHT:
            {
                SET_BIT(inputState, GISF_MOVING_RIGHT, e->isPressed());
                break;
            }
            case GPEB_STICK_LEFT:
            {
                SET_BIT(inputState, GISF_MOVING_UP, e->_y < 0);
                SET_BIT(inputState, GISF_MOVING_LEFT, e->_x < 0);
                SET_BIT(inputState, GISF_MOVING_DOWN, e->_y > 0);
                SET_BIT(inputState, GISF_MOVING_RIGHT, e->_x > 0);
                break;
            }
            default:
                break;
        }
    }
    
    for (KeyboardEvent* e : INPUT_MGR.getKeyboardEvents())
    {
        switch (e->_key)
        {
            case GOW_KEY_ESCAPE:
                _state = GIMS_EXIT;
                break;
            case GOW_KEY_SPACE_BAR:
                SET_BIT(inputStateP1, GISF_CONFIRM, e->isPressed());
                break;
            case GOW_KEY_DELETE:
            case GOW_KEY_BACK_SPACE:
                SET_BIT(inputStateP1, GISF_CANCEL, e->isPressed());
                break;
            case GOW_KEY_W:
                SET_BIT(inputStateP1, GISF_MOVING_UP, e->isPressed());
                break;
            case GOW_KEY_A:
                SET_BIT(inputStateP1, GISF_MOVING_LEFT, e->isPressed());
                break;
            case GOW_KEY_S:
                SET_BIT(inputStateP1, GISF_MOVING_DOWN, e->isPressed());
                break;
            case GOW_KEY_D:
                SET_BIT(inputStateP1, GISF_MOVING_RIGHT, e->isPressed());
                break;
            case GOW_KEY_CARRIAGE_RETURN:
                SET_BIT(inputStateP2, GISF_CONFIRM, e->isPressed());
                break;
            case GOW_KEY_COMMA:
                SET_BIT(inputStateP2, GISF_CANCEL, e->isPressed());
                break;
            case GOW_KEY_ARROW_UP:
                SET_BIT(inputStateP2, GISF_MOVING_UP, e->isPressed());
                break;
            case GOW_KEY_ARROW_LEFT:
                SET_BIT(inputStateP2, GISF_MOVING_LEFT, e->isPressed());
                break;
            case GOW_KEY_ARROW_DOWN:
                SET_BIT(inputStateP2, GISF_MOVING_DOWN, e->isPressed());
                break;
            case GOW_KEY_ARROW_RIGHT:
                SET_BIT(inputStateP2, GISF_MOVING_RIGHT, e->isPressed());
                break;
            case GOW_KEY_PERIOD:
            {
                if (e->isDown())
                {
                    drop2ndPlayer();
                }
                break;
            }
            default:
                break;
        }
    }
    
    if (_inputState->isRequestingToAddLocalPlayer())
    {
        NW_CLNT->requestToAddLocalPlayer();
    }
    
    return _state;
}

const Move& GameInputManager::sampleInputAsNewMove()
{
    InputState* inputState = _poolInputState.obtain();
    _inputState->copyTo(inputState);
    
    TimeTracker* tt = INST_REG.get<TimeTracker>(INSK_TIME_CLNT);
    return _moveList.addMove(inputState, tt->_time, NW_CLNT->getNumMovesProcessed());
}

InputState* GameInputManager::inputState()
{
    return _inputState;
}

MoveList& GameInputManager::moveList()
{
    return _moveList;
}

void GameInputManager::free(InputState *gis)
{
    if (gis == nullptr)
    {
        return;
    }
    
    _poolInputState.free(gis);
}

void GameInputManager::reset()
{
    free(_inputState);
    
    _inputState = _poolInputState.obtain();
    _inputState->reset();
    _inputState->setMaxNumPlayers(CFG_MAIN.maxNumPlayers());
    
    _state = GIMS_DEFAULT;
    _moveList.clear();
}

void GameInputManager::drop2ndPlayer()
{
    InputState::PlayerInputState& pis = _inputState->playerInputState(1);
    pis._playerID = 0;
    NW_CLNT->requestToDropLocalPlayer(1);
}

GameInputManager::GameInputManager() :
_state(GIMS_DEFAULT),
_inputState(nullptr),
_matrix(nullptr)
{
    reset();
}

GameInputManager::~GameInputManager()
{
    free(_inputState);
}
