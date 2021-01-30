//
//  GameInputState.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GameInputState.hpp"

#include "OutputMemoryBitStream.hpp"
#include "InputMemoryBitStream.hpp"
#include "MainConfig.hpp"
#include "Network.hpp"

#include <assert.h>

GameInputState::GameInputState() : InputState()
{
    _playerInputStates.resize(CFG_MAIN._maxNumPlayers);
}

void GameInputState::write(OutputMemoryBitStream& inOutputStream) const
{
    for (uint8_t i = 0; i < CFG_MAIN._maxNumPlayers; ++i)
    {
        bool playerID = _playerInputStates[i]._playerID != NW_INPUT_UNASSIGNED;
        inOutputStream.write(playerID);
        if (playerID)
        {
            _playerInputStates[i].write(inOutputStream);
        }
    }
}

void GameInputState::read(InputMemoryBitStream& inInputStream)
{
    for (uint8_t i = 0; i < CFG_MAIN._maxNumPlayers; ++i)
    {
        bool isInputAssignedBit;
        inInputStream.read(isInputAssignedBit);
        if (isInputAssignedBit)
        {
            _playerInputStates[i].read(inInputStream);
        }
    }
}

void GameInputState::reset()
{
    for (PlayerInputState& is : _playerInputStates)
    {
        is._playerID = NW_INPUT_UNASSIGNED;
        is._inputState = 0;
    }
}

bool GameInputState::isEqual(InputState* inputState) const
{
    GameInputState* in = static_cast<GameInputState*>(inputState);
    
    for (uint8_t i = 0; i < CFG_MAIN._maxNumPlayers; ++i)
    {
        if (in->_playerInputStates[i]._playerID != _playerInputStates[i]._playerID)
        {
            return false;
        }
        
        if (in->_playerInputStates[i]._inputState != _playerInputStates[i]._inputState)
        {
            return false;
        }
    }
    
    return true;
}

void GameInputState::copyTo(InputState* inputState) const
{
    GameInputState* gameInputState = static_cast<GameInputState*>(inputState);
    
    for (uint8_t i = 0; i < CFG_MAIN._maxNumPlayers; ++i)
    {
        gameInputState->_playerInputStates[i]._playerID = _playerInputStates[i]._playerID;
        gameInputState->_playerInputStates[i]._inputState = _playerInputStates[i]._inputState;
    }
}

void GameInputState::activateNextPlayer(uint8_t playerID)
{
    for (uint8_t i = 0; i < CFG_MAIN._maxNumPlayers; ++i)
    {
        if (_playerInputStates[i]._playerID == NW_INPUT_UNASSIGNED)
        {
            _playerInputStates[i]._playerID = playerID;
            return;
        }
    }
}

GameInputState::PlayerInputState* GameInputState::getPlayerInputStateForID(uint8_t playerID)
{
    for (uint8_t i = 0; i < CFG_MAIN._maxNumPlayers; ++i)
    {
        if (_playerInputStates[i]._playerID == playerID)
        {
            return &_playerInputStates[i];
        }
    }
    
    return NULL;
}

bool GameInputState::isRequestingToAddLocalPlayer() const
{
    for (int i = 1; i < CFG_MAIN._maxNumPlayers; ++i)
    {
        if (_playerInputStates[i]._playerID == NW_INPUT_UNASSIGNED &&
            _playerInputStates[i]._inputState > 0)
        {
            return true;
        }
    }
    
    return false;
}

GameInputState::PlayerInputState& GameInputState::getPlayerInputState(int index)
{
    return _playerInputStates[index];
}

void GameInputState::PlayerInputState::write(OutputMemoryBitStream& inOutputStream) const
{
    inOutputStream.write<uint8_t, 3>(_playerID);
    inOutputStream.write<uint8_t, 5>(_inputState);
}

void GameInputState::PlayerInputState::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.read<uint8_t, 3>(_playerID);
    inInputStream.read<uint8_t, 5>(_inputState);
}

uint8_t GameInputState::PlayerInputState::getInputState()
{
    return _inputState;
}

GameInputState::PlayerInputState::PlayerInputState() :
_playerID(NW_INPUT_UNASSIGNED),
_inputState(0)
{
    // Empty
}
