//
//  CrystalController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/v/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "CrystalController.hpp"

#include "Entity.hpp"
#include "GameInputState.hpp"
#include "Rektangle.hpp"
#include "World.hpp"
#include "Macros.hpp"
#include "TimeTracker.hpp"
#include "StringUtil.hpp"
#include "MathUtil.hpp"
#include "NetworkManagerServer.hpp"
#include "NetworkManagerClient.hpp"
#include "GameInputManager.hpp"
#include "GowAudioEngine.hpp"
#include "SoundUtil.hpp"
#include "Config.hpp"
#include "MainConfig.hpp"
#include "GameEngineState.hpp"
#include "Macros.hpp"
#include "Server.hpp"
#include "PlayerController.hpp"
#include "MathUtil.hpp"
#include "InstanceRegistry.hpp"
#include "EntityIDManager.hpp"
#include "Network.hpp"
#include "EntityManager.hpp"

IMPL_RTTI(CrystalController, EntityController)
IMPL_EntityController_create(CrystalController)

void CrystalController::update()
{
    _entity->pose()._velocity.mul(0.86f);
    sanitizeCloseToZeroVector(_entity->pose()._velocity._x, _entity->pose()._velocity._y, 0.01f);
}

void CrystalController::onMessage(uint16_t message, void* data)
{
    switch (message)
    {
        case MSG_ENCOUNTER:
        {
            if (_entity->networkController()->isServer())
            {
                uint32_t networkID = INST_REG.get<EntityIDManager>(INSK_EID_SRVR)->getNextNetworkEntityID();
                EntityInstanceDef eid(networkID, 'EXPL', _entity->getPosition()._x, _entity->getPosition()._y);
                NW_MGR_SRVR->registerEntity(ENTITY_MGR.createEntity(eid, true));
                _entity->requestDeletion();
            }
            break;
        }
        default:
            break;
    }
}

void CrystalController::push(int dir)
{
    static float pushSpeed = 6;
    
    switch (dir)
    {
        case PDIR_UP:
            _entity->pose()._velocity._y = pushSpeed;
            break;
        case PDIR_DOWN:
            _entity->pose()._velocity._y = -pushSpeed;
            break;
        case PDIR_LEFT:
            _entity->pose()._velocity._x = -pushSpeed;
            break;
        case PDIR_RIGHT:
            _entity->pose()._velocity._x = pushSpeed;
            break;
        default:
            break;
    }
}
