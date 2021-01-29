//
//  MainInputManager.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#define INPUT_MAIN MainInputManager::getInstance()

enum MainInputManagerState
{
    MainInputManagerState_DEFAULT =    0,
    MainInputManagerState_EXIT =       1 << 0,
    MainInputManagerState_PLAY_SOUND = 1 << 1
};

class MainInputManager
{
public:
    static MainInputManager& getInstance();
    
    MainInputManagerState update();
    
private:
    MainInputManagerState _state;
    
    MainInputManager();
    ~MainInputManager();
    MainInputManager(const MainInputManager&);
    MainInputManager& operator=(const MainInputManager&);
};
