//
//  MainConfig.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "MainConfig.hpp"

#include "Config.hpp"

MainConfig& MainConfig::getInstance()
{
    static MainConfig ret = MainConfig();
    return ret;
}

void MainConfig::init()
{
    Config c;
    c.initWithJSONFile("json/config.json");
    
    _framebufferWidth = c.getInt("framebufferWidth");
    _framebufferHeight = c.getInt("framebufferHeight");
    _camWidth = c.getInt("camWidth");
    _camHeight = c.getInt("camHeight");
    _maxTextInputLength = c.getInt("maxTextInputLength");
}
