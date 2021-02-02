//
//  MainEngineController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "MainEngineController.hpp"

#include "Network.hpp"
#include "TimeTracker.hpp"
#include "EntityIDManager.hpp"
#include "InstanceManager.hpp"
#include "TitleEngineState.hpp"
#include "EntityMapper.hpp"
#include "HidePlayerController.hpp"
#include "JackiePlayerController.hpp"
#include "MonsterController.hpp"
#include "CrystalController.hpp"
#include "OvenController.hpp"
#include "MainConfig.hpp"

MainEngineController::MainEngineController() :
_frameRate(0)
{
    CFG_MAIN.init();
    
    _frameRate = 1.0 / CFG_MAIN._framesPerSecond;
    
    static TimeTracker TIMS(_frameRate);
    INSTANCE_MGR.registerInstance(INSK_TIMING_SERVER, &TIMS);
    static TimeTracker TIMC(_frameRate);
    INSTANCE_MGR.registerInstance(INSK_TIMING_CLIENT, &TIMC);
    static EntityIDManager EIMS;
    INSTANCE_MGR.registerInstance(INSK_ENTITY_ID_MANAGER_SERVER, &EIMS);
    static EntityIDManager EIMC;
    INSTANCE_MGR.registerInstance(INSK_ENTITY_ID_MANAGER_CLIENT, &EIMC);
    
    ENTITY_MAPPER.registerFunction("Hide", HidePlayerController::create);
    ENTITY_MAPPER.registerFunction("Hide", HidePlayerNetworkController::create);
    ENTITY_MAPPER.registerFunction("Jackie", JackiePlayerController::create);
    ENTITY_MAPPER.registerFunction("Jackie", JackiePlayerNetworkController::create);
    ENTITY_MAPPER.registerFunction("Monster", MonsterController::create);
    ENTITY_MAPPER.registerFunction("Monster", MonsterNetworkController::create);
    ENTITY_MAPPER.registerFunction("Crystal", CrystalController::create);
    ENTITY_MAPPER.registerFunction("Crystal", CrystalNetworkController::create);
    ENTITY_MAPPER.registerFunction("Oven", OvenController::create);
    ENTITY_MAPPER.registerFunction("Oven", OvenNetworkController::create);
}

State<Engine>* MainEngineController::getInitialState()
{
    return &ENGINE_STATE_MAIN;
}

double MainEngineController::getFrameRate()
{
    return _frameRate;
}
