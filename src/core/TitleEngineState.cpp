//
//  TitleEngineState.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "TitleEngineState.hpp"

#include "Engine.hpp"
#include "Assets.hpp"
#include "TitleInputManager.hpp"
#include "GowAudioEngine.hpp"
#include "GameClientEngineState.hpp"
#include "GameHostEngineState.hpp"
#include "GameServerEngineState.hpp"
#include "StringUtil.hpp"
#include "MainConfig.hpp"

#include <stdlib.h>
#include <assert.h>

void TitleEngineState::enter(Engine* e)
{
    createDeviceDependentResources();
    onWindowSizeChanged(e->screenWidth(), e->screenHeight(), e->cursorWidth(), e->cursorHeight());
}

void TitleEngineState::execute(Engine* e)
{
    switch (e->requestedStateAction())
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

void TitleEngineState::exit(Engine* e)
{
    releaseDeviceDependentResources();
    
    _state = MESS_DEFAULT;
    _stateTime = 0;
    _userEnteredIPAddress.clear();
    INPUT_MAIN.clearTextInput();
}

void TitleEngineState::createDeviceDependentResources()
{
    ASSETS.initWithJSONFile("assets/json/assets_title.json");
    
    _renderer.createDeviceDependentResources();
    GOW_AUDIO.createDeviceDependentResources();
    GOW_AUDIO.setSoundsDisabled(CFG_MAIN._sfxDisabled);
    GOW_AUDIO.setMusicDisabled(CFG_MAIN._musicDisabled);
    
    GOW_AUDIO.playMusic(true, 0.1f);
}

void TitleEngineState::onWindowSizeChanged(int screenWidth, int screenHeight, int cursorWidth, int cursorHeight)
{
    _renderer.onWindowSizeChanged(screenWidth, screenHeight);
}

void TitleEngineState::releaseDeviceDependentResources()
{
    _renderer.releaseDeviceDependentResources();
    GOW_AUDIO.releaseDeviceDependentResources();
}

void TitleEngineState::resume()
{
    GOW_AUDIO.resume();
}

void TitleEngineState::pause()
{
    GOW_AUDIO.pause();
}

void TitleEngineState::update(Engine* e)
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
        case MESS_START_DEDICATED_SERVER:
            updateStartDedicatedServer(e);
            break;
        default:
            break;
    }
}

void TitleEngineState::updateDefault(Engine* e)
{
    TitleInputManagerState mims = INPUT_MAIN.update(MIMU_DEFAULT);
    switch (mims)
    {
        case MIMS_EXIT:
            e->setRequestedHostAction(ERHA_EXIT);
            break;
        case MIMS_START_SRVR:
            _state = MESS_START_DEDICATED_SERVER;
            break;
        case MIMS_HOST_SRVR:
            _state = MESS_INPUT_HOST_NAME;
            break;
        case MIMS_JOIN_SRVR:
            _state = MESS_INPUT_IP;
            break;
        default:
            break;
    }
}

void TitleEngineState::updateInputIP(Engine* e)
{
    TitleInputManagerState mims = INPUT_MAIN.update(MIMU_READ_TEXT);
    switch (mims)
    {
        case MIMS_EXIT:
            _state = MESS_DEFAULT;
            break;
        case MIMS_TEXT_INPUT_READY:
            _userEnteredIPAddress = INPUT_MAIN.getTextInput();
            INPUT_MAIN.clearTextInput();
            _state = MESS_INPUT_JOIN_NAME;
            break;
        default:
            break;
    }
}

void TitleEngineState::updateInputHostName(Engine* e)
{
    TitleInputManagerState mims = INPUT_MAIN.update(MIMU_READ_TEXT);
    switch (mims)
    {
        case MIMS_EXIT:
            _state = MESS_DEFAULT;
            break;
        case MIMS_TEXT_INPUT_READY:
        {
            Config args;
            args.getMap().insert({ARG_USERNAME, INPUT_MAIN.getTextInput()});
            e->changeState(&ENGINE_STATE_GAME_HOST, args);
            break;
        }
        default:
            break;
    }
}

void TitleEngineState::updateInputJoinName(Engine* e)
{
    TitleInputManagerState mims = INPUT_MAIN.update(MIMU_READ_TEXT);
    switch (mims)
    {
        case MIMS_EXIT:
            _state = MESS_INPUT_IP;
            INPUT_MAIN.setTextInput(_userEnteredIPAddress);
            break;
        case MIMS_TEXT_INPUT_READY:
        {
            Config args;
            args.getMap().insert({ARG_IP_ADDRESS, _userEnteredIPAddress});
            args.getMap().insert({ARG_USERNAME, INPUT_MAIN.getTextInput()});
            e->changeState(&ENGINE_STATE_GAME_CLNT, args);
            break;
        }
        default:
            break;
    }
}

void TitleEngineState::updateStartDedicatedServer(Engine* e)
{
    ++_stateTime;
    if (_stateTime > 10)
    {
        e->changeState(&ENGINE_STATE_GAME_SRVR);
    }
}

void TitleEngineState::render()
{
    _renderer.render();
    GOW_AUDIO.render();
}

TitleEngineState::TitleEngineState() : State<Engine>(),
_renderer(),
_state(MESS_DEFAULT),
_stateTime(0)
{
    // Empty
}
