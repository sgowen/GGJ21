//
//  CrystalController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/v/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "CrystalController.hpp"

#include "Entity.hpp"
#include "InputState.hpp"
#include "Rektangle.hpp"
#include "World.hpp"
#include "Macros.hpp"
#include "TimeTracker.hpp"
#include "StringUtil.hpp"
#include "MathUtil.hpp"
#include "NetworkServer.hpp"
#include "NetworkClient.hpp"
#include "GameInputManager.hpp"
#include "AudioEngineFactory.hpp"
#include "SoundUtil.hpp"
#include "Config.hpp"
#include "MainConfig.hpp"
#include "Macros.hpp"
#include "PlayerController.hpp"
#include "MathUtil.hpp"
#include "InstanceRegistry.hpp"
#include "EntityIDManager.hpp"
#include "Network.hpp"
#include "EntityManager.hpp"
#include "TopDownPhysicsController.hpp"

IMPL_RTTI(CrystalController, EntityController)
IMPL_EntityController_create(CrystalController, EntityController)

void CrystalController::onMessage(uint16_t message)
{
    switch (message)
    {
        case MSG_ENCOUNTER:
        {
            // TODO, client should be able to create entities locally
            // and have everything line up correctly on the registry side
            // Only way to do that though... is to sync _nextNetworkEntityID
            if (_entity->isServer())
            {
                NW_SRVR->registerEntity(ENTITY_MGR.createEntity(EntityInstanceDef(INST_REG.get<EntityIDManager>(INSK_EID_SRVR)->getNextNetworkEntityID(), 'EXPL', _entity->position()._x, _entity->position()._y, true)));
                NW_SRVR->registerEntity(ENTITY_MGR.createEntity(EntityInstanceDef(INST_REG.get<EntityIDManager>(INSK_EID_SRVR)->getNextNetworkEntityID(), 'MON1', rand() % 24 + 6, rand() % 32 + 12, true)));
                _entity->requestDeletion();
            }
            break;
        }
        default:
            break;
    }
}
