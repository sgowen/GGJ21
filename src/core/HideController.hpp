//
//  HideController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <GowEngine/GowEngine.hpp>

#include "PlayerController.hpp"

class HideController : public PlayerController
{
    DECL_RTTI;
    DECL_EntityController_create(EntityController);
    
public:
    HideController(Entity* e);
    virtual ~HideController();
    
    virtual void update() override;
    virtual void onMessage(uint16_t message) override;
    virtual void processInput(InputState* is, bool isLive) override;
    
    Entity* battleAvatar();
    
private:
    enum BattleState
    {
        BSTT_IDLE = 0,
        BSTT_SWING = 1
    };
    
    Entity* _battleAvatar;
};
