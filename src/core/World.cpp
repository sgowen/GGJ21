//
//  World.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

World::World() :
_entityLayout()
{
    // Empty
}

World::~World()
{
    reset();
}

void World::populateFromEntityLayout(EntityLayoutDef& eld)
{
    reset();
    
    _entityLayout = eld;
    
    for (auto& eid : _entityLayout._entities)
    {
        addEntity(ENTITY_MGR.createEntity(eid));
    }
}

void World::addNetworkEntity(Entity* e)
{
    assert(!e->isLayer() && !e->isStatic());
    
    TopDownPhysicsController* epc = e->physicsController<TopDownPhysicsController>();
    epc->initPhysics();
    
    if (e->isPlayer())
    {
        _players.push_back(e);
    }
    else if (e->isDynamic())
    {
        _networkEntities.push_back(e);
    }
}

void World::removeNetworkEntity(Entity* e)
{
    assert(!e->isLayer() && !e->isStatic());
    
    if (e->isPlayer())
    {
        removeEntity(e, _players);
    }
    else if (e->isDynamic())
    {
        removeEntity(e, _networkEntities);
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
        TopDownPhysicsController* epc = e->physicsController<TopDownPhysicsController>();
        epc->processPhysics(tt);
    }
    
    for (Entity* e : _networkEntities)
    {
        TopDownPhysicsController* epc = e->physicsController<TopDownPhysicsController>();
        epc->processPhysics(tt);
    }
    
    for (Entity* e : _players)
    {
        TopDownPhysicsController* epc = e->physicsController<TopDownPhysicsController>();
        epc->processCollisions(_networkEntities);
        epc->processCollisions(_staticEntities);
    }
    
    for (Entity* e : _networkEntities)
    {
        TopDownPhysicsController* epc = e->physicsController<TopDownPhysicsController>();
        epc->processCollisions(_networkEntities);
        epc->processCollisions(_staticEntities);
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

void World::reset()
{
    _entityLayout = EntityLayoutDef();
    
    STLUtil::cleanUpVectorOfPointers(_layers);
    STLUtil::cleanUpVectorOfPointers(_staticEntities);
}

bool World::isEntityLayoutLoaded()
{
    return _entityLayout._key > 0;
}

EntityLayoutDef& World::getEntityLayout()
{
    return _entityLayout;
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

void World::addEntity(Entity *e)
{
    assert(!e->isDynamic());
    
    TopDownPhysicsController* epc = e->physicsController<TopDownPhysicsController>();
    epc->initPhysics();
    
    if (e->isLayer())
    {
        _layers.push_back(e);
    }
    else if (e->isStatic())
    {
        _staticEntities.push_back(e);
    }
}

void World::removeEntity(Entity* e)
{
    assert(!e->isDynamic());
    
    if (e->isLayer())
    {
        removeEntity(e, _layers);
    }
    else if (e->isStatic())
    {
        removeEntity(e, _staticEntities);
    }
}

void World::removeEntity(Entity* e, std::vector<Entity*>& entities)
{
    assert(e != nullptr);
    
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
