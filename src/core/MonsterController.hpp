//
//  MonsterController.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <GowEngine/GowEngine.hpp>

class MonsterController : public EntityController
{
    friend class MonsterNetworkController;
    friend class MonsterRenderController;
    
    DECL_RTTI;
    DECL_EntityController_create(EntityController);
    
public:
    MonsterController(Entity* e);
    virtual ~MonsterController() {}
    
    virtual void update() override;
    virtual void onCollision(Entity* e) override;
    
    bool isInEncounter();
    
private:
    enum State
    {
        STAT_IDLE   = 0,
        STAT_MOVING = 1
    };
    Entity* _battleAvatar;
};

class MonsterRenderController : public EntityRenderController
{
    DECL_RTTI;
    DECL_EntityController_create(EntityRenderController);
    
public:
    MonsterRenderController(Entity* e) : EntityRenderController(e) {}
    virtual ~MonsterRenderController() {}
    
    virtual std::string getTextureMapping() override;
    
    void addSpriteForEncounter(SpriteBatcher& sb);

private:
    std::string getTextureMappingForEncounter();
};
