//
//  GameInputState.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "InputState.hpp"

#include <stdint.h>
#include <vector>

enum GameInputStateFlags
{
    GISF_MOVING_RIGHT = 1 << 0,
    GISF_MOVING_LEFT =  1 << 1,
    GISF_MOVING_UP =    1 << 2,
    GISF_MOVING_DOWN =  1 << 3,
    GISF_CONFIRM     =  1 << 4,
    GISF_CANCEL      =  1 << 5
};

class OutputMemoryBitStream;
class InputMemoryBitStream;

class GameInputState : public InputState
{    
    friend class GameInputManager;
    
public:
    class PlayerInputState
    {
        friend class GameInputState;
        friend class GameInputManager;
        
    public:
        PlayerInputState();
        
        void write(OutputMemoryBitStream& ombs) const;
        void read(InputMemoryBitStream& imbs);
        uint8_t playerID();
        uint8_t inputState();
        
    private:
        uint8_t _playerID;
        uint8_t _inputState;
    };
    
    GameInputState();
    
    virtual void write(OutputMemoryBitStream& ombs) const;
    virtual void read(InputMemoryBitStream& imbs);
    virtual void reset();
    virtual bool isEqual(InputState* inputState) const;
    virtual void copyTo(InputState* inputState) const;
    
    void activateNextPlayer(uint8_t playerID);
    PlayerInputState* getPlayerInputStateForID(uint8_t playerID);
    bool isRequestingToAddLocalPlayer() const;
    PlayerInputState& getPlayerInputState(int index);
    
private:
    std::vector<PlayerInputState> _playerInputStates;
};
