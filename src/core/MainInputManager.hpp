//
//  MainInputManager.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <string>
#include <stdint.h>

enum MainInputManagerState
{
    MIMS_DEFAULT,
    MIMS_EXIT,
    MIMS_START_SERVER,
    MIMS_JOIN_SERVER,
    MIMS_TEXT_INPUT_READY
};

enum MainInputManagerUpdateArg
{
    MIMU_DEFAULT,
    MIMU_READ_TEXT
};

#define INPUT_MAIN MainInputManager::getInstance()

class MainInputManager
{
public:
    static MainInputManager& getInstance();
    
    MainInputManagerState update(MainInputManagerUpdateArg mimu);
    std::string getTextInput();
    void setTextInput(std::string textInput);
    void clearTextInput();
    
private:
    MainInputManagerState _state;
    std::string _textInput;
    
    void updateDefault();
    void updateReadText();
    void acceptKeyInput(uint16_t key);
    
    MainInputManager();
    ~MainInputManager() {}
    MainInputManager(const MainInputManager&);
    MainInputManager& operator=(const MainInputManager&);
};
