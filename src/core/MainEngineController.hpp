//
//  MainEngineController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "EngineController.hpp"

class MainEngineController : public EngineController
{
public:
    MainEngineController(void* data1 = NULL, void* data2 = NULL);
    virtual ~MainEngineController() {}
    
    virtual State<Engine>& getInitialState();
    virtual double getFrameRate();
};
