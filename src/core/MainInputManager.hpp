//
//  MainInputManager.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#define INPUT_MAIN MainInputManager::getInstance()

class MainInputManager
{
public:
    static MainInputManager& getInstance();
    
    void update();
    
private:    
    MainInputManager();
    ~MainInputManager();
    MainInputManager(const MainInputManager&);
    MainInputManager& operator=(const MainInputManager&);
};
