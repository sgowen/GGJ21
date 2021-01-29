//
//  MainEngineState.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "MainEngineState.hpp"

#include "Engine.hpp"

#include "Assets.hpp"
#include "Macros.hpp"
#include "MainInputManager.hpp"
#include "MainConfig.hpp"
#include "GowAudioEngine.hpp"

#include <stdlib.h>
#include <assert.h>

MainEngineState& MainEngineState::getInstance()
{
    static MainEngineState ret = MainEngineState();
    return ret;
}

void MainEngineState::enter(Engine* e)
{
    createDeviceDependentResources();
    onWindowSizeChanged(e->screenWidth(), e->screenHeight(), e->cursorWidth(), e->cursorHeight());
}

void MainEngineState::execute(Engine* e)
{
    // Empty
}

void MainEngineState::exit(Engine* e)
{
    releaseDeviceDependentResources();
}

void MainEngineState::createDeviceDependentResources()
{
    CFG_MAIN.init();
    ASSETS.initWithJSONFile("json/assets_main.json");
    
    _renderer.createDeviceDependentResources();
    GowAudioEngine::create();
    GOW_AUDIO->loadFromAssets();
}

void MainEngineState::onWindowSizeChanged(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight)
{
    _renderer.onWindowSizeChanged(screenWidth, screenHeight);
}

void MainEngineState::releaseDeviceDependentResources()
{
    _renderer.releaseDeviceDependentResources();
    GowAudioEngine::destroy();
}

void MainEngineState::onResume()
{
    GOW_AUDIO->resume();
}

void MainEngineState::onPause()
{
    GOW_AUDIO->pause();
}

void MainEngineState::update()
{
    MainInputManagerState mims = INPUT_MAIN.update();
    if (mims == MainInputManagerState_PLAY_SOUND)
    {
        GOW_AUDIO->playSound(1);
    }
}

void MainEngineState::render(double alpha)
{
    UNUSED(alpha);
    
    _renderer.render();
    GOW_AUDIO->render();
}

MainEngineState::MainEngineState() : EngineState(),
_renderer()
{
    // Empty
}

MainEngineState::~MainEngineState()
{
    // Empty
}
