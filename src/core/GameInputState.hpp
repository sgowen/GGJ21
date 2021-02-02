//
//  GameInputState.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <stdint.h>
#include <vector>

#include "InputState.hpp"

enum GameInputStateFlags
{
    GISF_MOVING_RIGHT = 1 << 0,
    GISF_MOVING_LEFT =  1 << 1,
    GISF_MOVING_UP =    1 << 2,
    GISF_MOVING_DOWN =  1 << 3
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
        
        void write(OutputMemoryBitStream& inOutputStream) const;
        void read(InputMemoryBitStream& inInputStream);
        uint8_t getInputState();
        
    private:
        uint8_t _playerID;
        uint8_t _inputState;
    };
    
    GameInputState();
    
    virtual void write(OutputMemoryBitStream& inOutputStream) const;
    virtual void read(InputMemoryBitStream& inInputStream);
    virtual void reset();
    virtual bool isEqual(InputState* inputState) const;
    virtual void copyTo(InputState* inputState) const;
    
    void activateNextPlayer(uint8_t playerID);
    PlayerInputState* getPlayerInputStateForID(uint8_t playerID);
    bool isRequestingToAddLocalPlayer() const;
    
private:
    std::vector<PlayerInputState> _playerInputStates;
    
    PlayerInputState& getPlayerInputState(int index);
};
