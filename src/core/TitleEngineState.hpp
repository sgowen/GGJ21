//
//  TitleEngineState.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "StateMachine.hpp"

#include "Renderer.hpp"

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

class TitleEngineState : public State<Engine>
{
    friend class TitleRenderer;
    
public:
    static TitleEngineState& getInstance()
    {
        static TitleEngineState ret = TitleEngineState();
        return ret;
    }
    
    virtual void enter(Engine* e);
    virtual void execute(Engine* e);
    virtual void exit(Engine* e);
    
private:    
    Renderer _renderer;
    TitleEngineStateState _state;
    std::string _userEnteredIPAddress;
    int _stateTime;
    
    void createDeviceDependentResources();
    void onWindowSizeChanged(uint16_t screenWidth, uint16_t screenHeight);
    void releaseDeviceDependentResources();
    void resume();
    void pause();
    void update(Engine* e);
    void updateDefault(Engine* e);
    void updateInputIP(Engine* e);
    void updateInputHostName(Engine* e);
    void updateInputJoinName(Engine* e);
    void updateStartDedicatedServer(Engine* e);
    void render();
    
    TitleEngineState();
    ~TitleEngineState() {}
    TitleEngineState(const TitleEngineState&);
    TitleEngineState& operator=(const TitleEngineState&);
};
