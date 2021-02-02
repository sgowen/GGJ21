//
//  TitleInputManager.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "TitleInputManager.hpp"

#include "InputManager.hpp"
#include "MainConfig.hpp"
#include "StringUtil.hpp"

TitleInputManagerState TitleInputManager::update(TitleInputManagerUpdate mimu)
{
    _state = MIMS_DEFAULT;
    
    switch (mimu)
    {
        case MIMU_DEFAULT:
            updateDefault();
            break;
        case MIMU_READ_TEXT:
            updateReadText();
            break;
        default:
            break;
    }
    
    return _state;
}

std::string TitleInputManager::getTextInput()
{
    return _textInput;
}

void TitleInputManager::setTextInput(std::string textInput)
{
    _textInput = textInput;
}

void TitleInputManager::clearTextInput()
{
    _textInput.clear();
}

void TitleInputManager::updateDefault()
{
#if IS_MOBILE
    for (CursorEvent* e : INPUT_MGR.getCursorEvents())
    {
        if (!e->isDown())
        {
            continue;
        }
        
        _state = MIMS_START_SERVER;
        break;
    }
#endif
    
    for (GamepadEvent* e : INPUT_MGR.getGamepadEvents())
    {
        if (!e->isDown())
        {
            continue;
        }
        
        switch (e->_button)
        {
            case GPEB_BUTTON_A:
                _state = MIMS_START_SERVER;
                continue;
            case GPEB_BUTTON_X:
                _state = MIMS_JOIN_SERVER;
                continue;
            case GPEB_BUTTON_SELECT:
                _state = MIMS_EXIT;
                continue;
            default:
                continue;
        }
    }
    
    for (KeyboardEvent* e : INPUT_MGR.getKeyboardEvents())
    {
        if (!e->isDown())
        {
            continue;
        }
        
        switch (e->_key)
        {
            case GOW_KEY_S:
                _state = MIMS_START_SERVER;
                continue;
            case GOW_KEY_J:
                _state = MIMS_JOIN_SERVER;
                continue;
            case GOW_KEY_ESCAPE:
                _state = MIMS_EXIT;
                continue;
            default:
                continue;
        }
    }
}

void TitleInputManager::updateReadText()
{
    for (CursorEvent* e : INPUT_MGR.getCursorEvents())
    {
        if (!e->isDown())
        {
            continue;
        }
        
        _state = MIMS_TEXT_INPUT_READY;
        _textInput = "mobile";
        break;
    }
    
    for (GamepadEvent* e : INPUT_MGR.getGamepadEvents())
    {
        if (!e->isDown())
        {
            continue;
        }
        
        switch (e->_button)
        {
            case GPEB_BUTTON_A:
                _state = MIMS_TEXT_INPUT_READY;
                _textInput = "gamer";
                continue;
            case GPEB_BUTTON_SELECT:
                _state = MIMS_EXIT;
                clearTextInput();
                continue;
            default:
                continue;
        }
    }
    
    for (KeyboardEvent* e : INPUT_MGR.getKeyboardEvents())
    {
        if (!e->isDown())
        {
            continue;
        }
        
        switch (e->_key)
        {
            case GOW_KEY_CARRIAGE_RETURN:
                _state = MIMS_TEXT_INPUT_READY;
                continue;
            case GOW_KEY_BACK_SPACE:
            case GOW_KEY_DELETE:
            {
                if (_textInput.end() > _textInput.begin())
                {
                    _textInput.erase(_textInput.end() - 1, _textInput.end());
                }
                continue;
            }
            case GOW_KEY_ESCAPE:
                _state = MIMS_EXIT;
                clearTextInput();
                continue;
            default:
                acceptKeyInput(e->_key);
                continue;
        }
    }
}

void TitleInputManager::acceptKeyInput(uint16_t key)
{
    if (_textInput.length() > CFG_MAIN._maxTextInputLength)
    {
        return;
    }
    
    char c = key;
    _textInput += StringUtil::format("%c", c);
}

TitleInputManager::TitleInputManager() :
_state(MIMS_DEFAULT),
_textInput("")
{
    // Empty
}
