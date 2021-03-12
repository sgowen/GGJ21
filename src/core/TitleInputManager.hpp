//
//  TitleInputManager.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <GowEngine/GowEngine.hpp>

enum TitleInputManagerState
{
    MIMS_DEFAULT,
    MIMS_EXIT,
    MIMS_START_SRVR,
    MIMS_HOST_SRVR,
    MIMS_JOIN_SRVR,
    MIMS_TEXT_INPUT_READY
};

#define INPUT_TITLE TitleInputManager::getInstance()

class TitleInputManager
{
public:
    static TitleInputManager& getInstance()
    {
        static TitleInputManager ret = TitleInputManager();
        return ret;
    }
    
    TitleInputManagerState update();
    TitleInputManagerState updateReadText();
    std::string getTextInput();
    void setTextInput(std::string textInput);
    void clearTextInput();
    
private:
    std::string _textInput;
    bool _isControlHeldDown;
    
    void acceptKeyInput(uint16_t key);
    
    TitleInputManager();
    ~TitleInputManager() {}
    TitleInputManager(const TitleInputManager&);
    TitleInputManager& operator=(const TitleInputManager&);
};
