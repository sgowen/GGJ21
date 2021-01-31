//
//  MainConfig.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#define CFG_MAIN MainConfig::getInstance()

#define MSG_ENCOUNTER 0x01u

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
    int _maxTextInputLength;
    int _maxNumPlayers;
    float _playerMaxTopDownSpeed;
    float _monsterMaxTopDownSpeed;
    float _monsterLineOfSight;
    bool _showDebug;
    float _playerBattleX;
    float _playerBattleY;
    float _playerBattleWidth;
    float _playerBattleHeight;
    float _monsterBattleX;
    float _monsterBattleY;
    float _monsterBattleWidth;
    float _monsterBattleHeight;
    
    void init();
    
private:
    MainConfig() {}
    ~MainConfig() {}
    MainConfig(const MainConfig&);
    MainConfig& operator=(const MainConfig&);
};
