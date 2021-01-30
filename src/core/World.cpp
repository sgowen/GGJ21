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

#include "EntityMapper.hpp"
#include "EntityLayoutMapper.hpp"
#include "EntityNetworkController.hpp"
#include "InstanceManager.hpp"
#include "MainConfig.hpp"
#include "Timing.hpp"
#include "StringUtil.hpp"
#include "GowUtil.hpp"
#include "Network.hpp"
#include "Macros.hpp"

World::World(uint32_t flags) :
_flags(flags),
_entityIDManager(static_cast<EntityIDManager*>(INSTANCE_MGR.get(IS_BIT_SET(_flags, WorldFlag_Server) ? INSK_ENTITY_ID_MANAGER_SERVER : INSK_ENTITY_ID_MANAGER_CLIENT))),
_map()
{
    // Empty
}

World::~World()
{
    clear();
    
    GowUtil::cleanUpVectorOfPointers(_dynamicEntities);
}

void World::loadMap(uint32_t map)
{
    assert(map > 0);
    
    clear();
    
    _map.key = map;
    _map.name = StringUtil::stringFromFourChar(map);
    _map.fileName = ENTITY_LAYOUT_MAPPER.getJsonConfigFilePath(map);
    
    ENTITY_LAYOUT_MAPPER.loadEntityLayout(_map.key, _entityIDManager);
    
    EntityLayoutDef& eld = ENTITY_LAYOUT_MAPPER.getEntityLayoutDef();
    for (EntityInstanceDef eid : eld._entities)
    {
        EntityDef* ed = ENTITY_MAPPER.getEntityDef(eid._key);
        if (IS_BIT_SET(_flags, WorldFlag_Client) && isDynamic(*ed))
        {
            // On the client, Dynamic Entities must arrive via network
            continue;
        }
        
        Entity* e = ENTITY_MAPPER.createEntity(&eid, IS_BIT_SET(_flags, WorldFlag_Server));
        addEntity(e);
    }
}

void World::addEntity(Entity *e)
{
    if (isLayer(e))
    {
        _layers.push_back(e);
    }
    else if (isStatic(e))
    {
        _staticEntities.push_back(e);
    }
    else if (isDynamic(e))
    {
        _dynamicEntities.push_back(e);
        
        refreshPlayers();
    }
}

void World::removeEntity(Entity* e)
{
    if (isLayer(e))
    {
        removeEntity(e, _layers);
    }
    else if (isStatic(e))
    {
        removeEntity(e, _staticEntities);
    }
    else if (isDynamic(e))
    {
        removeEntity(e, _dynamicEntities);
        
        refreshPlayers();
    }
}

void World::stepPhysics()
{
    for (Entity* e : _players)
    {
        e->selfProcessPhysics();
    }
}

void World::clear()
{
    GowUtil::cleanUpVectorOfPointers(_layers);
    GowUtil::cleanUpVectorOfPointers(_staticEntities);
    
    if (IS_BIT_SET(_flags, WorldFlag_Server))
    {
        GowUtil::cleanUpVectorOfPointers(_dynamicEntities);
        refreshPlayers();
    }
}

bool World::isMapLoaded()
{
    return _map.key > 0;
}

std::string& World::getMapName()
{
    return _map.name;
}

std::string& World::getMapFileName()
{
    return _map.fileName;
}

std::vector<Entity*>& World::getPlayers()
{
    return _players;
}

std::vector<Entity*>& World::getDynamicEntities()
{
    return _dynamicEntities;
}

std::vector<Entity*>& World::getStaticEntities()
{
    return _staticEntities;
}

std::vector<Entity*>& World::getLayers()
{
    return _layers;
}

bool World::isLayer(Entity* e)
{
    return e->getEntityDef()._bodyFlags == 0;
}

bool World::isStatic(Entity* e)
{
    return IS_BIT_SET(e->getEntityDef()._bodyFlags, BODF_STATIC) && !e->getEntityDef()._stateSensitive;
}

bool World::isDynamic(Entity* e)
{
    return isDynamic(e->getEntityDef());
}

bool World::isDynamic(EntityDef& ed)
{
    return (ed._bodyFlags > 0 && !IS_BIT_SET(ed._bodyFlags, BODF_STATIC)) || ed._stateSensitive;
}

void World::refreshPlayers()
{
    _players.clear();
    
    for (Entity* e : _dynamicEntities)
    {
        if (IS_BIT_SET(e->getEntityDef()._bodyFlags, BODF_PLAYER))
        {
            _players.push_back(e);
        }
    }
}

void World::removeEntity(Entity* e, std::vector<Entity*>& entities)
{
    assert(e != NULL);
    
    for (std::vector<Entity*>::iterator i = entities.begin(); i != entities.end(); )
    {
        if (e->getID() == (*i)->getID())
        {
            delete *i;
            
            i = entities.erase(i);
            return;
        }
        else
        {
            ++i;
        }
    }
}
