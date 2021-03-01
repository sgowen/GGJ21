//
//  TitleEngineState.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <GowEngine/GowEngine.hpp>

enum TitleEngineStateState
{
    TESS_DEFAULT,
    TESS_INPUT_IP,
    TESS_INPUT_HOST_NAME,
    TESS_INPUT_JOIN_NAME,
    TESS_START_DEDICATED_SERVER,
};

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
    
    virtual void onEnter(Engine* e);
    virtual void onExit(Engine* e);
    virtual void onUpdate(Engine* e);
    
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
