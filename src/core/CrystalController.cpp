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

IMPL_RTTI(CrystalController, EntityController)
IMPL_EntityController_create(CrystalController)

void CrystalController::update()
{
    _entity->pose()._velocity.mul(0.86f);
    
    if (_entity->state()._state == STAT_EXPLODING)
    {
        if (_entity->state()._stateTime >= 42)
        {
            _entity->requestDeletion();
        }
    }
}

void CrystalController::onMessage(uint16_t message, void* data)
{
    switch (message)
    {
        case MSG_ENCOUNTER:
        {
            if (_entity->state()._state != STAT_EXPLODING)
            {
                _entity->state()._state = STAT_EXPLODING;
                _entity->state()._stateTime = 0;
                _entity->pose()._velocity.set(VECTOR2_ZERO);
                
                // Explosion animation frames are twice the size of the crystal
                _entity->pose()._width *= 2;
                _entity->pose()._height *= 2;
            }
            break;
        }
        default:
            break;
    }
}

void CrystalController::push(int dir)
{
    if (_entity->state()._state == STAT_EXPLODING)
    {
        return;
    }
    
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
