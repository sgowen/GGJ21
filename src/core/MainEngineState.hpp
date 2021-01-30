//
//  MainEngineState.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "StateMachine.hpp"

#include "MainRenderer.hpp"

enum MainEngineStateState
{
    MESS_DEFAULT,
    MESS_INPUT_IP,
    MESS_INPUT_HOST_NAME,
    MESS_INPUT_JOIN_NAME
};

class Engine;

#define ENGINE_STATE_MAIN MainEngineState::getInstance()

class MainEngineState : public State<Engine>
{
    friend class MainRenderer;
    
public:
    static MainEngineState& getInstance();
    
    virtual void enter(Engine* e);
    virtual void execute(Engine* e);
    virtual void exit(Engine* e);
    
private:    
    MainRenderer _renderer;
    MainEngineStateState _state;
    std::string _userEnteredIPAddress;
    
    void createDeviceDependentResources();
    void onWindowSizeChanged(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight);
    void releaseDeviceDependentResources();
    void resume();
    void pause();
    void update(Engine* e);
    void updateDefault(Engine* e);
    void updateInputIP(Engine* e);
    void updateInputHostName(Engine* e);
    void updateInputJoname(Engine* e);
    void render();
    
    MainEngineState();
    ~MainEngineState() {}
    MainEngineState(const MainEngineState&);
    MainEngineState& operator=(const MainEngineState&);
};
