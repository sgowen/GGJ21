//
//  MainEngineController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "MainEngineController.hpp"

#include "Network.hpp"
#include "Timing.hpp"
#include "EntityIDManager.hpp"
#include "InstanceManager.hpp"
#include "MainEngineState.hpp"
#include "MainConfig.hpp"

MainEngineController::MainEngineController()
{
    static Timing TIMS;
    INSTANCE_MGR.registerInstance(INSK_TIMING_SERVER, &TIMS);
    
    static Timing TIMC;
    INSTANCE_MGR.registerInstance(INSK_TIMING_CLIENT, &TIMC);
    
    static EntityIDManager EIMS;
    INSTANCE_MGR.registerInstance(INSK_ENTITY_ID_MANAGER_SERVER, &EIMS);
    
    static EntityIDManager EIMC;
    INSTANCE_MGR.registerInstance(INSK_ENTITY_ID_MANAGER_CLIENT, &EIMC);
    
    CFG_MAIN.init();
}

State<Engine>* MainEngineController::getInitialState()
{
    return &ENGINE_STATE_MAIN;
}
