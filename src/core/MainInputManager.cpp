//
//  MainInputManager.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "MainInputManager.hpp"

#include "InputManager.hpp"

MainInputManager& MainInputManager::getInstance()
{
    static MainInputManager ret = MainInputManager();
    return ret;
}

void MainInputManager::update()
{
    // Empty
}

MainInputManager::MainInputManager()
{
    // Empty
}

MainInputManager::~MainInputManager()
{
    // Empty
}
