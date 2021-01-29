//
//  MainEngineState.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "EngineState.hpp"

#include "MainRenderer.hpp"

class MainEngineState : public EngineState
{
public:
    static MainEngineState& getInstance();
    
    virtual void enter(Engine* e);
    virtual void execute(Engine* e);
    virtual void exit(Engine* e);
    
    virtual void createDeviceDependentResources();
    virtual void onWindowSizeChanged(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight);
    virtual void releaseDeviceDependentResources();
    virtual void onResume();
    virtual void onPause();
    virtual void update();
    virtual void render(double alpha);
    
private:    
    MainRenderer _renderer;
    
    MainEngineState();
    ~MainEngineState();
    MainEngineState(const MainEngineState&);
    MainEngineState& operator=(const MainEngineState&);
};
