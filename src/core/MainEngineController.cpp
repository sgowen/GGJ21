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

MainEngineController::MainEngineController(void* data1, void* data2) : EngineController(data1, data2)
{
    CFG_MAIN.init();
    
    static TimeTracker TIMS(getFrameRate());
    static TimeTracker TIMC(getFrameRate());
    static EntityIDManager EIMS;
    static EntityIDManager EIMC;
    
    INSTANCE_MGR.registerInstance(INSK_TIMING_SERVER, &TIMS);
    INSTANCE_MGR.registerInstance(INSK_TIMING_CLIENT, &TIMC);
    INSTANCE_MGR.registerInstance(INSK_ENTITY_ID_MANAGER_SERVER, &EIMS);
    INSTANCE_MGR.registerInstance(INSK_ENTITY_ID_MANAGER_CLIENT, &EIMC);
    
    ENTITY_MAPPER.registerEntityController("Hide",    HidePlayerController::create);
    ENTITY_MAPPER.registerEntityController("Jackie",  JackiePlayerController::create);
    ENTITY_MAPPER.registerEntityController("Monster", MonsterController::create);
    ENTITY_MAPPER.registerEntityController("Crystal", CrystalController::create);
    ENTITY_MAPPER.registerEntityController("Oven",    OvenController::create);
    
    ENTITY_MAPPER.registerEntityNetworkController("Hide",    HidePlayerNetworkController::create);
    ENTITY_MAPPER.registerEntityNetworkController("Jackie",  JackiePlayerNetworkController::create);
    ENTITY_MAPPER.registerEntityNetworkController("Monster", MonsterNetworkController::create);
}

State<Engine>* MainEngineController::getInitialState()
{
    return &ENGINE_STATE_TITLE;
}

double MainEngineController::getFrameRate()
{
    return 1.0 / CFG_MAIN._framesPerSecond;
}
