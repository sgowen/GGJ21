//
//  MainConfig.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <GowEngine/GowEngine.hpp>

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
    
    uint32_t framesPerSecond() { return _config.getUInt("framesPerSecond"); }
    uint32_t clientPortHost() { return _config.getUInt("clientPortHost"); }
    uint32_t clientPortJoin() { return _config.getUInt("clientPortJoin"); }
    uint32_t serverPort() { return _config.getUInt("serverPort"); }
    uint32_t maxTextInputLength() { return _config.getUInt("maxTextInputLength"); }
    uint32_t maxNumPlayers() { return _config.getUInt("maxNumPlayers"); }
    float playerMaxTopDownSpeed() { return _config.getFloat("playerMaxTopDownSpeed"); }
    float monsterMaxTopDownSpeed() { return _config.getFloat("monsterMaxTopDownSpeed"); }
    float monsterLineOfSight() { return _config.getFloat("monsterLineOfSight"); }
    bool showDebug() { return _config.getBool("showDebug"); }
    float playerBattleX() { return _config.getFloat("playerBattleX"); }
    float playerBattleY() { return _config.getFloat("playerBattleY"); }
    float jackiePushForce() { return _config.getFloat("jackiePushForce"); }
    float monsterBattleX() { return _config.getFloat("monsterBattleX"); }
    float monsterBattleY() { return _config.getFloat("monsterBattleY"); }
    float monsterBattleWidth() { return _config.getFloat("monsterBattleWidth"); }
    float monsterBattleHeight() { return _config.getFloat("monsterBattleHeight"); }
    bool soundsDisabled() { return _config.getBool("soundsDisabled"); }
    bool musicDisabled() { return _config.getBool("musicDisabled"); }
    bool inputLoggingEnabled() { return _config.getBool("inputLoggingEnabled"); }
    bool networkLoggingEnabled() { return _config.getBool("networkLoggingEnabled"); }
    std::string entityManagerFilePath() { return _config.getString("entityManagerFilePath"); }
    std::string entityLayoutManagerFilePath() { return _config.getString("entityLayoutManagerFilePath"); }
    
private:
    Config _config;
    
    MainConfig() : _config(ConfigLoader::initWithJSONFile("data/json/config.json")) {}
    ~MainConfig() {}
    MainConfig(const MainConfig&);
    MainConfig& operator=(const MainConfig&);
};
