//
//  MainInputManager.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "MainInputManager.hpp"

#include "InputManager.hpp"
#include "KeyboardLookup.hpp"
#include "MainConfig.hpp"
#include "StringUtil.hpp"

#include <sstream>

MainInputManager& MainInputManager::getInstance()
{
    static MainInputManager ret = MainInputManager();
    return ret;
}

MainInputManagerState MainInputManager::update(MainInputManagerUpdateArg mimu)
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

std::string MainInputManager::getTextInput()
{
    return _textInput;
}

void MainInputManager::setTextInput(std::string textInput)
{
    _textInput = textInput;
}

void MainInputManager::clearTextInput()
{
    _textInput.clear();
}

void MainInputManager::updateDefault()
{
#if IS_MOBILE
    for (CursorEvent* e : INPUT_MGR.getCursorEvents())
    {
        if (!e->isPressed())
        {
            continue;
        }
        
        _state = MIMS_START_SERVER;
        break;
    }
#endif
    
    for (GamepadEvent* e : INPUT_MGR.getGamepadEvents())
    {
        if (!e->isPressed())
        {
            continue;
        }
        
        switch (e->_type)
        {
            case GPET_BUTTON_A:
                _state = MIMS_START_SERVER;
                continue;
            case GPET_BUTTON_X:
                _state = MIMS_JOIN_SERVER;
                continue;
            case GPET_BUTTON_SELECT:
                _state = MIMS_EXIT;
                continue;
            default:
                continue;
        }
    }
    
    for (KeyboardEvent* e : INPUT_MGR.getKeyboardEvents())
    {
        if (!e->isPressed())
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

void MainInputManager::updateReadText()
{
#if IS_MOBILE
    for (CursorEvent* e : INPUT_MGR.getCursorEvents())
    {
        if (!e->isPressed())
        {
            continue;
        }
        
        _state = MIMS_TEXT_INPUT_READY;
        _textInput = "mobile";
        break;
    }
#endif
    
    for (GamepadEvent* e : INPUT_MGR.getGamepadEvents())
    {
        if (!e->isPressed())
        {
            continue;
        }
        
        switch (e->_type)
        {
            case GPET_BUTTON_A:
                _state = MIMS_TEXT_INPUT_READY;
                _textInput = "gamer";
                continue;
            case GPET_BUTTON_SELECT:
                _state = MIMS_EXIT;
                clearTextInput();
                continue;
            default:
                continue;
        }
    }
    
    std::stringstream ss;
    for (KeyboardEvent* e : INPUT_MGR.getKeyboardEvents())
    {
        if (!e->isPressed())
        {
            continue;
        }
        
        if (!isKeySupported(e->_key))
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

void MainInputManager::acceptKeyInput(uint16_t key)
{
    if (_textInput.length() > CFG_MAIN._maxTextInputLength)
    {
        return;
    }
    
    char c = key;
    _textInput += StringUtil::format("%c", c);
}

MainInputManager::MainInputManager() :
_state(MIMS_DEFAULT),
_textInput("")
{
    // Empty
}
