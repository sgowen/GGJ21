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
    void stepPhysics(TimeTracker* tt);
    void clearLayout();
    void clearNetwork();
    
    bool isEntityLayoutLoaded();
    uint32_t getEntityLayoutKey();
    std::string& getEntityLayoutName();
    std::string& getEntityLayoutFilePath();
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
    
    bool isLayer(Entity* e);
    bool isStatic(Entity* e);
    bool isDynamic(Entity* e);
    bool isPlayer(Entity* e);
    void addEntity(Entity* e);
    void removeEntity(Entity* e);
    void removeEntity(Entity* e, std::vector<Entity*>& entities);
};
