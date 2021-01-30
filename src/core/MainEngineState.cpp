//
//  MainEngineState.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "MainEngineState.hpp"

#include "Engine.hpp"

#include "Assets.hpp"
#include "MainInputManager.hpp"
#include "GowAudioEngine.hpp"
#include "GameEngineState.hpp"
#include "StringUtil.hpp"
#include "MainConfig.hpp"

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
    
    _state = MESS_DEFAULT;
    _userEnteredIPAddress.clear();
    INPUT_MAIN.clearTextInput();
}

void MainEngineState::execute(Engine* e)
{
    switch (e->state())
    {
        case ERSA_CREATE_RESOURCES:
            createDeviceDependentResources();
            break;
        case ERSA_WINDOW_SIZE_CHANGED:
            onWindowSizeChanged(e->screenWidth(), e->screenHeight(), e->cursorWidth(), e->cursorHeight());
            break;
        case ERSA_RELEASE_RESOURCES:
            releaseDeviceDependentResources();
            break;
        case ERSA_RESUME:
            resume();
            break;
        case ERSA_PAUSE:
            pause();
            break;
        case ERSA_UPDATE:
            update(e);
            break;
        case ERSA_RENDER:
            render();
            break;
        case ERSA_DEFAULT:
        default:
            break;
    }
}

void MainEngineState::exit(Engine* e)
{
    releaseDeviceDependentResources();
}

void MainEngineState::createDeviceDependentResources()
{
    ASSETS.initWithJSONFile("json/assets_main.json");
    _renderer.createDeviceDependentResources();
    GOW_AUDIO.createDeviceDependentResources();
    // TODO, remove
    GOW_AUDIO.playSound(1);
}

void MainEngineState::onWindowSizeChanged(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight)
{
    _renderer.onWindowSizeChanged(screenWidth, screenHeight);
}

void MainEngineState::releaseDeviceDependentResources()
{
    ASSETS.clear();
    _renderer.releaseDeviceDependentResources();
    GOW_AUDIO.releaseDeviceDependentResources();
}

void MainEngineState::resume()
{
    GOW_AUDIO.resume();
}

void MainEngineState::pause()
{
    GOW_AUDIO.pause();
}

void MainEngineState::update(Engine* e)
{
    switch (_state)
    {
        case MESS_DEFAULT:
            updateDefault(e);
            break;
        case MESS_INPUT_IP:
            updateInputIP(e);
            break;
        case MESS_INPUT_HOST_NAME:
            updateInputHostName(e);
            break;
        case MESS_INPUT_JOIN_NAME:
            updateInputJoinName(e);
            break;
        default:
            break;
    }
}

void MainEngineState::updateDefault(Engine* e)
{
    MainInputManagerState mims = INPUT_MAIN.update(MIMU_DEFAULT);
    switch (mims)
    {
        case MIMS_EXIT:
            e->setRequestedHostAction(ERHA_EXIT);
            break;
        case MIMS_START_SERVER:
            _state = MESS_INPUT_HOST_NAME;
            break;
        case MIMS_JOIN_SERVER:
            _state = MESS_INPUT_IP;
            break;
        default:
            break;
    }
}

void MainEngineState::updateInputIP(Engine* e)
{
    MainInputManagerState mims = INPUT_MAIN.update(MIMU_READ_TEXT);
    switch (mims)
    {
        case MIMS_EXIT:
            _state = MESS_DEFAULT;
            break;
        case MIMS_TEXT_INPUT_READY:
            _userEnteredIPAddress = StringUtil::format("%s:%d", INPUT_MAIN.getTextInput().c_str(), CFG_MAIN._serverPort);
            INPUT_MAIN.clearTextInput();
            _state = MESS_INPUT_JOIN_NAME;
            break;
        default:
            break;
    }
}

void MainEngineState::updateInputHostName(Engine* e)
{
    MainInputManagerState mims = INPUT_MAIN.update(MIMU_READ_TEXT);
    switch (mims)
    {
        case MIMS_EXIT:
            _state = MESS_DEFAULT;
            break;
        case MIMS_TEXT_INPUT_READY:
            GameEngineState::sHandleHostServer(e, INPUT_MAIN.getTextInput());
            break;
        default:
            break;
    }
}

void MainEngineState::updateInputJoinName(Engine* e)
{
    MainInputManagerState mims = INPUT_MAIN.update(MIMU_READ_TEXT);
    switch (mims)
    {
        case MIMS_EXIT:
            _state = MESS_INPUT_IP;
            INPUT_MAIN.setTextInput(_userEnteredIPAddress);
            break;
        case MIMS_TEXT_INPUT_READY:
            GameEngineState::sHandleJoinServer(e, _userEnteredIPAddress, INPUT_MAIN.getTextInput());
            break;
        default:
            break;
    }
}

void MainEngineState::render()
{
    _renderer.render();
    GOW_AUDIO.render();
}

MainEngineState::MainEngineState() : State<Engine>(),
_renderer(),
_state(MESS_DEFAULT)
{
    // Empty
}
