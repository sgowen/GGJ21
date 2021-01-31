//
//  HidePlayerController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "HidePlayerController.hpp"

#include "MainConfig.hpp"

#include <assert.h>

IMPL_RTTI(HidePlayerController, PlayerController);
IMPL_EntityController_create(HidePlayerController);

HidePlayerController::HidePlayerController(Entity* e) : PlayerController(e)
{
    // Empty
}

void HidePlayerController::onMessage(uint16_t message, void* data)
{
    // TODO
    
    switch (message)
    {
        default:
            PlayerController::onMessage(message, data);
            break;
    }
}

std::string HidePlayerController::getTextureMapping(uint8_t state)
{
    switch (_stats._dir)
    {
        case PDIR_UP:
            return "HIDE_UP";
        case PDIR_LEFT:
        case PDIR_RIGHT:
            return "HIDE_LEFT";
        case PDIR_DOWN:
            return "HIDE_DOWN";
        default:
            assert(false);
    }
}

std::string HidePlayerController::getTextureMappingForEncounter()
{
    switch (_encounter._state)
    {
        case ESTA_IDLE:
            return "BIG_HIDE_IDLE";
        case ESTA_SWING:
            return _encounter._stateTime >= 35 ? "BIG_HIDE_SWING_3" : _encounter._stateTime >= 14 ? "BIG_HIDE_SWING_2" : "BIG_HIDE_SWING_1";
        default:
            assert(false);
    }
}

float HidePlayerController::getWidthForEncounter()
{
    switch (_encounter._state)
    {
        case ESTA_IDLE:
            return CFG_MAIN._playerBattleWidth;
        case ESTA_SWING:
            return _encounter._stateTime >= 35 ? CFG_MAIN._playerBattleWidth : _encounter._stateTime >= 14 ? (CFG_MAIN._playerBattleWidth + 4) : CFG_MAIN._playerBattleWidth;
        default:
            assert(false);
    }
}

IMPL_EntityNetworkController_create(HidePlayerNetworkController);

HidePlayerNetworkController::HidePlayerNetworkController(Entity* e, bool isServer) : PlayerNetworkController(e, isServer)
{
    // Empty
}
