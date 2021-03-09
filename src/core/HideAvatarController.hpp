//
//  HideAvatarController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <GowEngine/GowEngine.hpp>
#include "GGJ21.hpp"

class HideAvatarController : public EntityController
{
    DECL_RTTI;
    DECL_EntityController_create(EntityController);
    
public:
    HideAvatarController(Entity* e) : EntityController(e) {}
    virtual ~HideAvatarController() {}
    
    virtual void update() override;
    virtual void onMessage(uint16_t message) override;
    
    void processInput(InputState::PlayerInputState* pis, bool isLive);
    
private:
    enum State
    {
        STAT_IDLE = 0,
        STAT_SWING = 1
    };
    enum EncounterState
    {
        ESTA_IDLE = 0,
        ESTA_SWING = 1
    };
};
