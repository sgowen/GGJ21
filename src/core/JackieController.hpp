//
//  JackieController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "PlayerController.hpp"

class JackieController : public PlayerController
{
    friend class JackiePhysicsController;
    friend class JackieRenderController;
    
    DECL_RTTI;
    DECL_EntityController_create(EntityController);
    
public:
    JackieController(Entity* e) : PlayerController(e) {}
    virtual ~JackieController() {}
    
    virtual void processInput(InputState* inputState, bool isLive) override;
};

#include "TopDownEntityPhysicsController.hpp"

class JackiePhysicsController : public TopDownEntityPhysicsController
{
    DECL_RTTI;
    DECL_EntityController_create(EntityPhysicsController);
    
public:
    JackiePhysicsController(Entity* e) : TopDownEntityPhysicsController(e) {}
    virtual ~JackiePhysicsController() {}
    
protected:
    virtual void onCollision(Entity* e) override;
};

#include "EntityRenderController.hpp"

class JackieRenderController : public EntityRenderController
{
    DECL_RTTI;
    DECL_EntityController_create(EntityRenderController);
    
public:
    JackieRenderController(Entity* e) : EntityRenderController(e) {}
    virtual ~JackieRenderController() {}
    
    virtual std::string getTextureMapping() override;
};
