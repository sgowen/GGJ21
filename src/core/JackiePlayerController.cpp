//
//  JackiePlayerController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "JackiePlayerController.hpp"

IMPL_RTTI(JackiePlayerController, PlayerController);
IMPL_EntityController_create(JackiePlayerController);

JackiePlayerController::JackiePlayerController(Entity* e) : PlayerController(e)
{
    // Empty
}

IMPL_EntityNetworkController_create(JackiePlayerNetworkController);

JackiePlayerNetworkController::JackiePlayerNetworkController(Entity* e, bool isServer) : PlayerNetworkController(e, isServer)
{
    // Empty
}
