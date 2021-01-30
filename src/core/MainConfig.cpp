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
    
    _serverPort = c.getInt("serverPort");
    _clientPort = c.getInt("clientPort");
    _framebufferWidth = c.getInt("framebufferWidth");
    _framebufferHeight = c.getInt("framebufferHeight");
    _camWidth = c.getInt("camWidth");
    _camHeight = c.getInt("camHeight");
    _maxTextInputLength = c.getInt("maxTextInputLength");
    _maxNumPlayers = c.getInt("maxNumPlayers");
    _playerMaxSpeed = c.getFloat("playerMaxSpeed");
    _showDebug = c.getBool("showDebug");
}
