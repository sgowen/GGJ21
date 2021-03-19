//
//  TitleInputManager.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

TitleInputManagerState TitleInputManager::update()
{
#if IS_MOBILE
    for (CursorEvent* e : INPUT_MGR.getCursorEvents())
    {
        if (!e->isDown())
        {
            continue;
        }
        
        return MIMS_HOST_SRVR;
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
            case GPEB_BUTTON_START:
            case GPEB_BUTTON_SNES_START:
                return MIMS_HOST_SRVR;
            case GPEB_BUTTON_SELECT:
            case GPEB_BUTTON_SNES_SELECT:
                return MIMS_EXIT;
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
                return MIMS_START_SRVR;
            case GOW_KEY_H:
                return MIMS_HOST_SRVR;
            case GOW_KEY_J:
                return MIMS_JOIN_SRVR;
            case GOW_KEY_ESCAPE:
                return MIMS_EXIT;
            default:
                continue;
        }
    }
    
    return MIMS_DEFAULT;
}

TitleInputManagerState TitleInputManager::updateReadText()
{
#if IS_MOBILE
    for (CursorEvent* e : INPUT_MGR.getCursorEvents())
    {
        if (!e->isDown())
        {
            continue;
        }
        
        _textInput = "mobile";
        return MIMS_TEXT_INPUT_READY;
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
            case GPEB_BUTTON_START:
            case GPEB_BUTTON_SNES_START:
                _textInput = "gamer";
                return MIMS_TEXT_INPUT_READY;
            case GPEB_BUTTON_SELECT:
            case GPEB_BUTTON_SNES_SELECT:
                clearTextInput();
                return MIMS_EXIT;
            default:
                continue;
        }
    }
    
    for (KeyboardEvent* e : INPUT_MGR.getKeyboardEvents())
    {
        if (e->_key == GOW_KEY_CMD || e->_key == GOW_KEY_CTRL)
        {
            _isControlHeldDown = e->isPressed();
        }
        
        if (!e->isDown())
        {
            continue;
        }
        
        switch (e->_key)
        {
            case GOW_KEY_CARRIAGE_RETURN:
                return MIMS_TEXT_INPUT_READY;
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
                clearTextInput();
                return MIMS_EXIT;
            case GOW_KEY_V:
                if (_isControlHeldDown)
                {
                    const char* clipboard = CLIPBOARD_UTIL.getClipboardString();
                    _textInput += clipboard;
                    if (_textInput.length() >= CFG_MAIN.maxTextInputLength())
                    {
                        int remove = (int)_textInput.length() - CFG_MAIN.maxTextInputLength();
                        _textInput.erase(_textInput.end() - remove, _textInput.end());
                    }
                    continue;
                }
            default:
                if (e->_isChar)
                {
                    acceptKeyInput(e->_key);
                }
                continue;
        }
    }
    
    return MIMS_DEFAULT;
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

void TitleInputManager::acceptKeyInput(uint16_t key)
{
    if (_textInput.length() >= CFG_MAIN.maxTextInputLength())
    {
        return;
    }
    
    char c = static_cast<char>(key);
    _textInput += StringUtil::format("%c", c);
}

TitleInputManager::TitleInputManager() :
_textInput(""),
_isControlHeldDown(false)
{
    // Empty
}
