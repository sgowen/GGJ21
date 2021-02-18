//
//  World.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "World.hpp"

#include "Entity.hpp"
#include "EntityIDManager.hpp"
#include "TimeTracker.hpp"
#include "Rektangle.hpp"
#include "EntityManager.hpp"
#include "EntityLayoutManager.hpp"
#include "EntityNetworkController.hpp"
#include "InstanceRegistry.hpp"
#include "MainConfig.hpp"
#include "StringUtil.hpp"
#include "GowUtil.hpp"
#include "Network.hpp"
#include "Macros.hpp"
#include "PlayerController.hpp"
#include "OverlapTester.hpp"
#include "TopDownPhysicsController.hpp"

World::World() :
_entityLayout()
{
    // Empty
}

World::~World()
{
    clearLayout();
    clearNetwork();
}

void World::populateFromEntityLayout(EntityLayoutDef& eld)
{
    clearLayout();
    
    _entityLayout = eld;
    
    for (auto& eid : _entityLayout._entities)
    {
        addEntity(ENTITY_MGR.createEntity(eid));
    }
}

void World::addNetworkEntity(Entity* e)
{
    assert(!isLayer(e) && !isStatic(e));
    
    if (isDynamic(e))
    {
        _networkEntities.push_back(e);
    }
    else if (isPlayer(e))
    {
        _players.push_back(e);
    }
}

void World::removeNetworkEntity(Entity* e)
{
    assert(!isLayer(e) && !isStatic(e));
    
    if (isDynamic(e))
    {
        removeEntity(e, _networkEntities);
    }
    else if (isPlayer(e))
    {
        removeEntity(e, _players);
    }
}

void World::recallCache()
{
    for (Entity* e : _players)
    {
        e->networkController()->recallCache();
    }
    
    for (Entity* e : _networkEntities)
    {
        e->networkController()->recallCache();
    }
}

void World::stepPhysics(TimeTracker* tt)
{
    for (Entity* e : _players)
    {
        TopDownPhysicsController* c = static_cast<TopDownPhysicsController*>(e->physicsController());
        c->processPhysics(tt);
        c->processCollisions(_networkEntities);
    }
    
    for (Entity* e : _networkEntities)
    {
        TopDownPhysicsController* c = static_cast<TopDownPhysicsController*>(e->physicsController());
        c->processPhysics(tt);
        c->processCollisions(_players);
        c->processCollisions(_networkEntities);
    }
    
    for (Entity* e : _players)
    {
        TopDownPhysicsController* c = static_cast<TopDownPhysicsController*>(e->physicsController());
        c->processCollisions(_staticEntities);
    }
    
    for (Entity* e : _networkEntities)
    {
        TopDownPhysicsController* c = static_cast<TopDownPhysicsController*>(e->physicsController());
        c->processCollisions(_staticEntities);
    }
    
    // Enforce split screen bounds
    for (Entity* e : _players)
    {
        TopDownPhysicsController* phys = static_cast<TopDownPhysicsController*>(e->physicsController());
        PlayerController* ec = e->controller<PlayerController>();
        uint8_t playerID = ec->getPlayerID();
        if (playerID == 1)
        {
            Rektangle player1ScreenBounds(0, 0, CFG_MAIN._splitScreenBarX, CFG_MAIN._camHeight);
            phys->enforceBounds(player1ScreenBounds);
        }
        else if (playerID == 2)
        {
            Rektangle player2ScreenBounds(CFG_MAIN._splitScreenBarX + CFG_MAIN._splitScreenBarWidth, 0, CFG_MAIN._splitScreenBarX, CFG_MAIN._camHeight);
            phys->enforceBounds(player2ScreenBounds);
        }
    }
}

std::vector<Entity*> World::update()
{
    std::vector<Entity*> toDelete;
    for (Entity* e : _players)
    {
        e->update();
        if (e->isRequestingDeletion())
        {
            toDelete.push_back(e);
        }
    }
    
    for (Entity* e : _networkEntities)
    {
        e->update();
        if (e->isRequestingDeletion())
        {
            toDelete.push_back(e);
        }
    }
    
    return toDelete;
}

void World::clearLayout()
{
    _entityLayout = EntityLayoutDef();
    
    GowUtil::cleanUpVectorOfPointers(_layers);
    GowUtil::cleanUpVectorOfPointers(_staticEntities);
}

void World::clearNetwork()
{
    GowUtil::cleanUpVectorOfPointers(_networkEntities);
    GowUtil::cleanUpVectorOfPointers(_players);
}

bool World::isEntityLayoutLoaded()
{
    return _entityLayout._key > 0;
}

uint32_t World::getEntityLayoutKey()
{
    return _entityLayout._key;
}

std::string& World::getEntityLayoutName()
{
    return _entityLayout._name;
}

std::string& World::getEntityLayoutFilePath()
{
    return _entityLayout._filePath;
}

std::vector<Entity*>& World::getLayers()
{
    return _layers;
}

std::vector<Entity*>& World::getStaticEntities()
{
    return _staticEntities;
}

std::vector<Entity*>& World::getNetworkEntities()
{
    return _networkEntities;
}

std::vector<Entity*>& World::getPlayers()
{
    return _players;
}

bool World::isLayer(Entity* e)
{
    return e->entityDef()._bodyFlags == 0;
}

bool World::isStatic(Entity* e)
{
    return IS_BIT_SET(e->entityDef()._bodyFlags, BODF_STATIC);
}

bool World::isDynamic(Entity* e)
{
    return IS_BIT_SET(e->entityDef()._bodyFlags, BODF_DYNAMIC) &&
    !e->controller()->getRTTI().isDerivedFrom(PlayerController::rtti);
}

bool World::isPlayer(Entity* e)
{
    return e->controller()->getRTTI().isDerivedFrom(PlayerController::rtti);
}

void World::addEntity(Entity *e)
{
    assert(!isDynamic(e) && !isPlayer(e));
    
    if (isLayer(e))
    {
        _layers.push_back(e);
    }
    else if (isStatic(e))
    {
        _staticEntities.push_back(e);
    }
}

void World::removeEntity(Entity* e)
{
    assert(!isDynamic(e) && !isPlayer(e));
    
    if (isLayer(e))
    {
        removeEntity(e, _layers);
    }
    else if (isStatic(e))
    {
        removeEntity(e, _staticEntities);
    }
}

void World::removeEntity(Entity* e, std::vector<Entity*>& entities)
{
    assert(e != NULL);
    
    for (std::vector<Entity*>::iterator i = entities.begin(); i != entities.end(); ++i)
    {
        Entity* entityToDelete = *i;
        if (entityToDelete == e)
        {
            delete entityToDelete;
            
            entities.erase(i);
            return;
        }
    }
}
