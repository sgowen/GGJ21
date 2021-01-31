//
//  HidePlayerController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "HidePlayerController.hpp"

IMPL_RTTI(HidePlayerController, PlayerController);
IMPL_EntityController_create(HidePlayerController);

HidePlayerController::HidePlayerController(Entity* e) : PlayerController(e)
{
    // Empty
}

IMPL_EntityNetworkController_create(HidePlayerNetworkController);

HidePlayerNetworkController::HidePlayerNetworkController(Entity* e, bool isServer) : PlayerNetworkController(e, isServer)
{
    // Empty
}
