//
//  MainInputManager.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "MainInputManager.hpp"

#include "InputManager.hpp"
#include "KeyboardLookup.hpp"

MainInputManager& MainInputManager::getInstance()
{
    static MainInputManager ret = MainInputManager();
    return ret;
}

MainInputManagerState MainInputManager::update()
{
    _state = MainInputManagerState_DEFAULT;
    
    for (auto& e : INPUT_MGR.getCursorEvents())
    {
        if (e->isPressed())
        {
            _state = MainInputManagerState_PLAY_SOUND;
            break;
        }
    }
    
    for (auto& e : INPUT_MGR.getGamepadEvents())
    {
        if (e->isPressed())
        {
            _state = e->_type == GamepadEventType_BUTTON_SELECT ? MainInputManagerState_EXIT : MainInputManagerState_PLAY_SOUND;
            break;
        }
    }
    
    for (auto& e : INPUT_MGR.getKeyboardEvents())
    {
        if (e->isPressed())
        {
            _state = e->_key == GOW_KEY_ESCAPE ? MainInputManagerState_EXIT : MainInputManagerState_PLAY_SOUND;
            break;
        }
    }
    
    return _state;
}

MainInputManager::MainInputManager() :
_state(MainInputManagerState_DEFAULT)
{
    // Empty
}

MainInputManager::~MainInputManager()
{
    // Empty
}
