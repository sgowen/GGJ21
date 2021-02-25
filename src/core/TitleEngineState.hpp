//
//  TitleEngineState.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "EngineState.hpp"

enum TitleEngineStateState
{
    TESS_DEFAULT,
    TESS_INPUT_IP,
    TESS_INPUT_HOST_NAME,
    TESS_INPUT_JOIN_NAME,
    TESS_START_DEDICATED_SERVER,
};

class Engine;

#define ENGINE_STATE_TITLE TitleEngineState::getInstance()

class TitleEngineState : public EngineState
{
    friend class TitleRenderer;
    
public:
    static TitleEngineState& getInstance()
    {
        static TitleEngineState ret = TitleEngineState();
        return ret;
    }
    
    virtual void enter(Engine* e);
    virtual void exit(Engine* e);
    virtual void update(Engine* e);
    virtual void render(Engine* e);
    
private:
    TitleEngineStateState _state;
    std::string _userEnteredIPAddress;
    int _stateTime;
    
    void updateDefault(Engine* e);
    void updateInputIP(Engine* e);
    void updateInputHostName(Engine* e);
    void updateInputJoinName(Engine* e);
    void updateStartDedicatedServer(Engine* e);
    
    TitleEngineState();
    virtual ~TitleEngineState() {}
    TitleEngineState(const TitleEngineState&);
    TitleEngineState& operator=(const TitleEngineState&);
};
