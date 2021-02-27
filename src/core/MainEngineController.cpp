//
//  MainEngineController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "MainEngineController.hpp"

#include "Network.hpp"
#include "InstanceRegistry.hpp"
#include "TitleEngineState.hpp"
#include "EntityManager.hpp"
#include "HideController.hpp"
#include "JackieController.hpp"
#include "MonsterController.hpp"
#include "CrystalController.hpp"
#include "OvenController.hpp"
#include "MainConfig.hpp"
#include "TopDownPhysicsController.hpp"
#include "ExplosionController.hpp"
#include "InputManager.hpp"
#include "SocketUtil.hpp"
#include "AudioEngineFactory.hpp"

MainEngineController::MainEngineController(void* data1, void* data2) : EngineController(data1, data2)
{
    SOCKET_UTIL.setLoggingEnabled(CFG_MAIN.networkLoggingEnabled());
    INPUT_MGR.setLoggingEnabled(CFG_MAIN.inputLoggingEnabled());
    AUDIO_ENGINE.setSoundsDisabled(CFG_MAIN.soundsDisabled());
    AUDIO_ENGINE.setMusicDisabled(CFG_MAIN.musicDisabled());
    
    std::map<std::string, EntityControllerCreationFunc> config;
    config.emplace("Hide", HideController::create);
    config.emplace("Jackie", JackieController::create);
    config.emplace("Monster", MonsterController::create);
    config.emplace("Crystal", CrystalController::create);
    config.emplace("Explosion", ExplosionController::create);
    registerControllers(config, CFG_MAIN.entityManagerFilePath());
    
    std::map<std::string, EntityNetworkControllerCreationFunc> configNW;
    configNW.emplace("Hide", HideNetworkController::create);
    configNW.emplace("Jackie", PlayerNetworkController::create);
    configNW.emplace("Monster", MonsterNetworkController::create);
    configureForNetwork(configNW, CFG_MAIN.entityLayoutManagerFilePath());
    
    std::map<std::string, EntityPhysicsControllerCreationFunc> configPhysics;
    configPhysics.emplace("Default", TopDownPhysicsController::create);
    configPhysics.emplace("Jackie", JackiePhysicsController::create);
    configPhysics.emplace("Monster", MonsterPhysicsController::create);
    configPhysics.emplace("Oven", OvenPhysicsController::create);
    registerPhysicsControllers(configPhysics);
    
    std::map<std::string, EntityRenderControllerCreationFunc> configRender;
    configRender.emplace("Monster", MonsterRenderController::create);
    configRender.emplace("Hide", HideRenderController::create);
    configRender.emplace("Jackie", JackieRenderController::create);
    registerRenderControllers(configRender);
}

State<Engine>& MainEngineController::getInitialState()
{
    return ENGINE_STATE_TITLE;
}

double MainEngineController::getFrameRate()
{
    return 1.0 / CFG_MAIN.framesPerSecond();
}
