//
//  MainEngineController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

MainEngineController::MainEngineController(void* data1, void* data2) : EngineController(data1, data2)
{
    SOCKET_UTIL.setLoggingEnabled(CFG_MAIN.networkLoggingEnabled());
    INPUT_MGR.setLoggingEnabled(CFG_MAIN.inputLoggingEnabled());
    AUDIO_ENGINE.setSoundsDisabled(CFG_MAIN.soundsDisabled());
    AUDIO_ENGINE.setMusicDisabled(CFG_MAIN.musicDisabled());
    
    ASSETS.registerAssets("global", AssetsLoader::initWithJSONFile("data/json/assets_global.json"));
    
    std::map<std::string, EntityControllerCreationFunc> config;
    config.emplace("Hide", HideController::create);
    config.emplace("HideAvatar", HideAvatarController::create);
    config.emplace("Jackie", JackieController::create);
    config.emplace("Oven", OvenController::create);
    config.emplace("Monster", MonsterController::create);
    config.emplace("MonsterAvatar", MonsterAvatarController::create);
    config.emplace("Crystal", CrystalController::create);
    config.emplace("Explosion", ExplosionController::create);
    configureForNetwork(CFG_MAIN.entityManagerFilePath(), CFG_MAIN.entityLayoutManagerFilePath(), config);
    
    // TODO, physics should be registered via json file
    ENTITY_MGR.registerPhysicsController("Default", TopDownPhysicsController::create);
}

State<Engine>& MainEngineController::getInitialState()
{
    return ENGINE_STATE_TITLE;
}

double MainEngineController::getFrameRate()
{
    return 1.0 / CFG_MAIN.framesPerSecond();
}
