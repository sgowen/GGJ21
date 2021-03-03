//
//  PlayerController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <GowEngine/GowEngine.hpp>

#include <string>

class PlayerController : public EntityController
{
    friend class PlayerNetworkController;
    friend class PlayerRenderController;
    
    DECL_RTTI;
    DECL_EntityController_create(EntityController);
    
public:
    PlayerController(Entity* e);
    virtual ~PlayerController() {}
    
    virtual void update() override;
    virtual void processInput(InputState* is, bool isLive);
    
    void setUsername(std::string value);
    std::string getUsername() const;
    void setUserAddress(std::string value);
    std::string getUserAddress() const;
    uint16_t getHealth();
    
protected:
    enum State
    {
        STAT_IDLE = 0,
        STAT_MOVING = 1
    };
};

class PlayerRenderController : public EntityRenderController
{
    DECL_RTTI;
    DECL_EntityController_create(EntityRenderController);
    
public:
    PlayerRenderController(Entity* e) : EntityRenderController(e) {}
    virtual ~PlayerRenderController() {}
    
    virtual void addSprite(SpriteBatcher& sb) override;
};
