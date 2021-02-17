//
//  OvenController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/v/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "OvenController.hpp"

#include "Entity.hpp"
#include "GameInputState.hpp"
#include "Rektangle.hpp"
#include "World.hpp"
#include "Macros.hpp"
#include "TimeTracker.hpp"
#include "StringUtil.hpp"
#include "MathUtil.hpp"
#include "NetworkServer.hpp"
#include "NetworkClient.hpp"
#include "GameInputManager.hpp"
#include "GowAudioEngine.hpp"
#include "SoundUtil.hpp"
#include "Config.hpp"
#include "MainConfig.hpp"
#include "Macros.hpp"
#include "PlayerController.hpp"
#include "CrystalController.hpp"

IMPL_RTTI(OvenController, EntityController)
IMPL_EntityController_create(OvenController)

void OvenController::onCollision(Entity* e)
{
    if (e->controller()->getRTTI().isDerivedFrom(CrystalController::rtti))
    {
        e->message(MSG_ENCOUNTER);
    }
}
