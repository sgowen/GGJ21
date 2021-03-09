//
//  CrystalController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/v/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

IMPL_RTTI(CrystalController, EntityController)
IMPL_EntityController_create(CrystalController, EntityController)

void CrystalController::onMessage(uint16_t message)
{
    switch (message)
    {
        case MSG_ENCOUNTER:
        {
            // TODO, client should be able to create entities locally and have everything line up correctly on the registry side. Only way to do that though... is to sync _nextNetworkEntityID
            if (_entity->isServer())
            {
                NW_SRVR->registerNewEntity('EXPL', _entity->position()._x, _entity->position()._y);
                NW_SRVR->registerNewEntity('MON1', rand() % 24 + 6, rand() % 32 + 12);
                _entity->requestDeletion();
            }
            break;
        }
        default:
            break;
    }
}
