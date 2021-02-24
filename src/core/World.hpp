//
//  World.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "EntityLayoutManager.hpp"

#include <memory>
#include <vector>
#include <string>

class Entity;
class EntityIDManager;
class TimeTracker;
struct Rektangle;

class World
{
public:
    World();
    ~World();
    
    void populateFromEntityLayout(EntityLayoutDef& eld);
    void addNetworkEntity(Entity* e);
    void removeNetworkEntity(Entity* e);
    void recallCache();
    void stepPhysics(TimeTracker* tt);
    std::vector<Entity*> update();
    void reset();
    
    bool isEntityLayoutLoaded();
    EntityLayoutDef& getEntityLayout();
    std::vector<Entity*>& getLayers();
    std::vector<Entity*>& getStaticEntities();
    std::vector<Entity*>& getNetworkEntities();
    std::vector<Entity*>& getPlayers();
    
private:
    EntityLayoutDef _entityLayout;
    std::vector<Entity*> _layers;
    std::vector<Entity*> _staticEntities;
    std::vector<Entity*> _networkEntities;
    std::vector<Entity*> _players;
    
    void addEntity(Entity* e);
    void removeEntity(Entity* e);
    void removeEntity(Entity* e, std::vector<Entity*>& entities);
};
