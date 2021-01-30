//
//  World.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <memory>
#include <vector>
#include <string>

enum WorldFlags
{
    WorldFlag_Server = 1 << 0,
    WorldFlag_Client = 1 << 1
};

struct Map
{
    uint32_t _key;
    std::string _fileName;
    std::string _name;
    
    Map()
    {
        _key = 0;
        _fileName = "";
        _name = "";
    }
};

struct EntityDef;
class Entity;
class EntityIDManager;

class World
{
public:
    World(uint32_t flags = 0);
    ~World();
    
    void loadMap(uint32_t map);
    void addEntity(Entity* e);
    void removeEntity(Entity* e);
    void stepPhysics();
    void clear();
    
    bool isMapLoaded();
    std::string& getMapName();
    std::string& getMapFileName();
    std::vector<Entity*>& getPlayers();
    std::vector<Entity*>& getDynamicEntities();
    std::vector<Entity*>& getStaticEntities();
    std::vector<Entity*>& getLayers();
    
private:
    uint32_t _flags;
    EntityIDManager* _entityIDManager;
    Map _map;
    
    std::vector<Entity*> _layers;
    std::vector<Entity*> _staticEntities;
    std::vector<Entity*> _dynamicEntities;
    std::vector<Entity*> _players;
    
    bool isLayer(Entity* e);
    bool isStatic(Entity* e);
    bool isDynamic(Entity* e);
    bool isDynamic(EntityDef& ed);
    
    void refreshPlayers();
    void removeEntity(Entity* e, std::vector<Entity*>& entities);
};
