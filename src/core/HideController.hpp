//
//  HideController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <GowEngine/GowEngine.hpp>
#include "GGJ21.hpp"

class HideController : public PlayerController
{
    DECL_RTTI;
    DECL_EntityController_create(HideController);
    
public:
    HideController(Entity* e);
    virtual ~HideController();
    
    virtual void update() override;
    virtual void onMessage(uint16_t message) override;
    virtual void processInput(InputState* is, bool isLive) override;
    
    Entity* battleAvatar();
    
private:
    enum EncounterState
    {
        ESTA_IDLE = 0,
        ESTA_SWING = 1
    };
    Entity* _battleAvatar;
};
