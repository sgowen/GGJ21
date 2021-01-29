//
//  MainConfig.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/5/18.
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
    
    _framebufferWidth = c.getInt("FramebufferWidth");
    _framebufferHeight = c.getInt("FramebufferHeight");
    _camWidth = c.getInt("CamWidth");
    _camHeight = c.getInt("CamHeight");
}

MainConfig::MainConfig()
{
    // Empty
}

MainConfig::~MainConfig()
{
    // Empty
}
