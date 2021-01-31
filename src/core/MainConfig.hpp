//
//  MainConfig.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#define CFG_MAIN MainConfig::getInstance()

class MainConfig
{
public:
    static MainConfig& getInstance();
    
    int _clientPortHost;
    int _clientPortJoin;
    int _serverPort;
    int _framebufferWidth;
    int _framebufferHeight;
    int _camWidth;
    int _camHeight;
    float _splitScreenBarX;
    float _splitScreenBarY;
    float _splitScreenBarWidth;
    float _splitScreenBarHeight;
    float _hideSpawnX;
    float _hideSpawnY;
    float _jackieSpawnX;
    float _jackieSpawnY;
    int _maxTextInputLength;
    int _maxNumPlayers;
    float _playerMaxTopDownSpeed;
    float _monsterMaxTopDownSpeed;
    float _monsterLineOfSight;
    bool _showDebug;
    
    void init();
    
private:
    MainConfig() {}
    ~MainConfig() {}
    MainConfig(const MainConfig&);
    MainConfig& operator=(const MainConfig&);
};
