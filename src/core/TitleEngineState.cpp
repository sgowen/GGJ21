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
#include "AudioEngineFactory.hpp"
#include "GameClientEngineState.hpp"
#include "GameHostEngineState.hpp"
#include "GameServerEngineState.hpp"
#include "StringUtil.hpp"
#include "MainConfig.hpp"
#include "AssetManager.hpp"
#include "TitleRenderer.hpp"
#include "Renderer.hpp"

#include <stdlib.h>
#include <assert.h>

void TitleEngineState::enter(Engine* e)
{
    EngineState::enter(e);
    
    AUDIO_ENGINE.playMusic(0.1f, true);
}

void TitleEngineState::exit(Engine* e)
{
    EngineState::exit(e);
    
    _state = TESS_DEFAULT;
    _stateTime = 0;
    _userEnteredIPAddress.clear();
    INPUT_TITLE.clearTextInput();
}

void TitleEngineState::update(Engine* e)
{
    EngineState::update(e);
    
    switch (_state)
    {
        case TESS_DEFAULT:
            updateDefault(e);
            break;
        case TESS_INPUT_IP:
            updateInputIP(e);
            break;
        case TESS_INPUT_HOST_NAME:
            updateInputHostName(e);
            break;
        case TESS_INPUT_JOIN_NAME:
            updateInputJoinName(e);
            break;
        case TESS_START_DEDICATED_SERVER:
            updateStartDedicatedServer(e);
            break;
        default:
            break;
    }
}

void TitleEngineState::render(Engine* e)
{
    EngineState::render(e);
    
    TitleRenderer::render(_renderer);
}

void TitleEngineState::updateDefault(Engine* e)
{
    TitleInputManagerState mims = INPUT_TITLE.update(MIMU_DEFAULT);
    switch (mims)
    {
        case MIMS_EXIT:
            e->setRequestedHostAction(ERHA_EXIT);
            break;
        case MIMS_START_SRVR:
            _state = TESS_START_DEDICATED_SERVER;
            break;
        case MIMS_HOST_SRVR:
            _state = TESS_INPUT_HOST_NAME;
            break;
        case MIMS_JOIN_SRVR:
            _state = TESS_INPUT_IP;
            break;
        default:
            break;
    }
}

void TitleEngineState::updateInputIP(Engine* e)
{
    TitleInputManagerState mims = INPUT_TITLE.update(MIMU_READ_TEXT);
    switch (mims)
    {
        case MIMS_EXIT:
            _state = TESS_DEFAULT;
            break;
        case MIMS_TEXT_INPUT_READY:
            _userEnteredIPAddress = INPUT_TITLE.getTextInput();
            INPUT_TITLE.clearTextInput();
            _state = TESS_INPUT_JOIN_NAME;
            break;
        default:
            break;
    }
}

void TitleEngineState::updateInputHostName(Engine* e)
{
    TitleInputManagerState mims = INPUT_TITLE.update(MIMU_READ_TEXT);
    switch (mims)
    {
        case MIMS_EXIT:
            _state = TESS_DEFAULT;
            break;
        case MIMS_TEXT_INPUT_READY:
        {
            Config args;
            args.putString(ARG_USERNAME, INPUT_TITLE.getTextInput());
            e->changeState(&ENGINE_STATE_GAME_HOST, args);
            break;
        }
        default:
            break;
    }
}

void TitleEngineState::updateInputJoinName(Engine* e)
{
    TitleInputManagerState mims = INPUT_TITLE.update(MIMU_READ_TEXT);
    switch (mims)
    {
        case MIMS_EXIT:
            _state = TESS_INPUT_IP;
            INPUT_TITLE.setTextInput(_userEnteredIPAddress);
            break;
        case MIMS_TEXT_INPUT_READY:
        {
            Config args;
            args.putString(ARG_IP_ADDRESS, _userEnteredIPAddress);
            args.putString(ARG_USERNAME, INPUT_TITLE.getTextInput());
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

TitleEngineState::TitleEngineState() : EngineState("data/json/assets_title.json", "data/json/renderer_title.json"),
_state(TESS_DEFAULT),
_userEnteredIPAddress(""),
_stateTime(0)
{
    // Empty
}
