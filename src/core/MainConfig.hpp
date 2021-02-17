//
//  MainConfig.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "Config.hpp"

#define CFG_MAIN MainConfig::getInstance()

#define MSG_ENCOUNTER 0x01u

#define ARG_IP_ADDRESS "ARG_IP_ADDRESS"
#define ARG_USERNAME "ARG_USERNAME"

class MainConfig
{
public:
    static MainConfig& getInstance()
    {
        static MainConfig ret = MainConfig();
        return ret;
    }
    
    int _framesPerSecond;
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
    bool _sfxDisabled;
    bool _musicDisabled;
    bool _inputLoggingEnabled;
    bool _networkLoggingEnabled;
    std::string _entityManagerFilePath;
    std::string _entityLayoutManagerFilePath;

    void init()
    {
        Config c;
        c.initWithJSONFile("assets/json/config_main.json");
        
        _framesPerSecond = c.getInt("framesPerSecond");
        _clientPortHost = c.getInt("clientPortHost");
        _clientPortJoin = c.getInt("clientPortJoin");
        _serverPort = c.getInt("serverPort");
        _framebufferWidth = c.getInt("framebufferWidth");
        _framebufferHeight = c.getInt("framebufferHeight");
        _camWidth = c.getInt("camWidth");
        _camHeight = c.getInt("camHeight");
        _splitScreenBarX = c.getFloat("splitScreenBarX");
        _splitScreenBarY = c.getFloat("splitScreenBarY");
        _splitScreenBarWidth = c.getFloat("splitScreenBarWidth");
        _splitScreenBarHeight = c.getFloat("splitScreenBarHeight");
        _maxTextInputLength = c.getInt("maxTextInputLength");
        _maxNumPlayers = c.getInt("maxNumPlayers");
        _playerMaxTopDownSpeed = c.getFloat("playerMaxTopDownSpeed");
        _monsterMaxTopDownSpeed = c.getFloat("monsterMaxTopDownSpeed");
        _monsterLineOfSight = c.getFloat("monsterLineOfSight");
        _showDebug = c.getBool("showDebug");
        _playerBattleX = c.getFloat("playerBattleX");
        _playerBattleY = c.getFloat("playerBattleY");
        _playerBattleWidth = c.getFloat("playerBattleWidth");
        _playerBattleHeight = c.getFloat("playerBattleHeight");
        _monsterBattleX = c.getFloat("monsterBattleX");
        _monsterBattleY = c.getFloat("monsterBattleY");
        _monsterBattleWidth = c.getFloat("monsterBattleWidth");
        _monsterBattleHeight = c.getFloat("monsterBattleHeight");
        _sfxDisabled = c.getBool("sfxDisabled");
        _musicDisabled = c.getBool("musicDisabled");
        _inputLoggingEnabled = c.getBool("inputLoggingEnabled");
        _networkLoggingEnabled = c.getBool("networkLoggingEnabled");
        _entityManagerFilePath = c.getString("entityManagerFilePath");
        _entityLayoutManagerFilePath = c.getString("entityLayoutManagerFilePath");
    }
    
private:
    MainConfig() {}
    ~MainConfig() {}
    MainConfig(const MainConfig&);
    MainConfig& operator=(const MainConfig&);
};
