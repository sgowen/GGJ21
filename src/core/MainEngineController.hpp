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
    MainEngineController();
    virtual ~MainEngineController() {}
    
    virtual State<Engine>* getInitialState();
};
