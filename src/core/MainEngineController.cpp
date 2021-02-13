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
#include "HidePlayerController.hpp"
#include "JackiePlayerController.hpp"
#include "MonsterController.hpp"
#include "CrystalController.hpp"
#include "OvenController.hpp"
#include "MainConfig.hpp"
#include "EntityTopDownPhysicsController.hpp"

MainEngineController::MainEngineController(void* data1, void* data2) : EngineController(data1, data2)
{
    CFG_MAIN.init();
    
    std::map<std::string, EntityControllerCreationFunc> config;
    config.emplace("Hide", HidePlayerController::create);
    config.emplace("Jackie", JackiePlayerController::create);
    config.emplace("Monster", MonsterController::create);
    config.emplace("Crystal", CrystalController::create);
    config.emplace("Oven", OvenController::create);
    registerControllers(config);
    
    std::map<std::string, EntityNetworkControllerCreationFunc> configNW;
    configNW.emplace("Hide", HidePlayerNetworkController::create);
    configNW.emplace("Jackie", PlayerNetworkController::create);
    configNW.emplace("Monster", MonsterNetworkController::create);
    configureForNetwork(configNW);
    
    // TODO, don't like, should be able to set globally
    std::map<std::string, EntityPhysicsControllerCreationFunc> configPhysics;
    configPhysics.emplace("Entity", EntityTopDownPhysicsController::create);
    registerPhysicsControllers(configPhysics);
}

State<Engine>* MainEngineController::getInitialState()
{
    return &ENGINE_STATE_TITLE;
}

double MainEngineController::getFrameRate()
{
    return 1.0 / CFG_MAIN._framesPerSecond;
}
