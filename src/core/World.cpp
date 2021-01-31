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
#include "PlayerController.hpp"
#include "Rektangle.hpp"
#include "OverlapTester.hpp"
#include "Constants.hpp"

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
    
    _map._key = map;
    _map._name = StringUtil::stringFromFourChar(map);
    _map._fileName = ENTITY_LAYOUT_MAPPER.getJsonConfigFilePath(map);
    
    ENTITY_LAYOUT_MAPPER.loadEntityLayout(_map._key, _entityIDManager);
    
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
    for (Entity* e : _dynamicEntities)
    {
        e->selfProcessPhysics();
        
        float x = e->getPosition().x;
        float y = e->getPosition().y;
        float w = e->getWidth();
        float h = e->getHeight();
        Rektangle monsterBounds(x - w / 2, y - h / 2, w, h);
        
        for (Entity* se : _staticEntities)
        {
            float se_x = se->getPosition().x;
            float se_y = se->getPosition().y;
            float se_w = se->getWidth();
            float se_h = se->getHeight();
            Rektangle staticEntityBounds(se_x - se_w / 2, se_y - se_h / 2, se_w, se_h);
            
            if (OverlapTester::doRektanglesOverlap(monsterBounds, staticEntityBounds))
            {
                if (monsterBounds.right() >= staticEntityBounds.left() ||
                    monsterBounds.left() <= staticEntityBounds.right() ||
                    monsterBounds.top() >= staticEntityBounds.bottom() ||
                    monsterBounds.bottom() <= staticEntityBounds.top())
                {
                    b2Vec2 vel = e->getVelocity();
                    vel *= FRAME_RATE;
                    b2Vec2 pos = e->getPosition();
                    pos -= vel;
                    e->setPosition(pos);
                }
                break;
            }
        }
        
        for (Entity* pe : _players)
        {
            float pe_x = pe->getPosition().x;
            float pe_y = pe->getPosition().y;
            float pe_w = pe->getWidth();
            float pe_h = pe->getHeight();
            Rektangle playerBounds(pe_x - pe_w / 2, pe_y - pe_h / 2, pe_w, pe_h);
            
            if (OverlapTester::doRektanglesOverlap(monsterBounds, playerBounds))
            {
                if (monsterBounds.right() >= playerBounds.left() ||
                    monsterBounds.left() <= playerBounds.right() ||
                    monsterBounds.top() >= playerBounds.bottom() ||
                    monsterBounds.bottom() <= playerBounds.top())
                {
                    b2Vec2 vel = e->getVelocity();
                    vel *= FRAME_RATE;
                    b2Vec2 pos = e->getPosition();
                    pos -= vel;
                    e->setPosition(pos);
                }
                break;
            }
        }
    }
    
    for (Entity* e : _players)
    {
        e->selfProcessPhysics();
        
        float x = e->getPosition().x;
        float y = e->getPosition().y;
        float w = e->getWidth();
        float h = e->getHeight();
        Rektangle playerBounds(x - w / 2, y - h / 2, w, h);
        
        for (Entity* se : _staticEntities)
        {
            float se_x = se->getPosition().x;
            float se_y = se->getPosition().y;
            float se_w = se->getWidth();
            float se_h = se->getHeight();
            Rektangle staticEntityBounds(se_x - se_w / 2, se_y - se_h / 2, se_w, se_h);
            
            if (OverlapTester::doRektanglesOverlap(playerBounds, staticEntityBounds))
            {
                if (playerBounds.right() >= staticEntityBounds.left() ||
                    playerBounds.left() <= staticEntityBounds.right() ||
                    playerBounds.top() >= staticEntityBounds.bottom() ||
                    playerBounds.bottom() <= staticEntityBounds.top())
                {
                    b2Vec2 vel = e->getVelocity();
                    vel *= FRAME_RATE;
                    b2Vec2 pos = e->getPosition();
                    pos -= vel;
                    e->setPosition(pos);
                }
                break;
            }
        }
        
        for (Entity* de : _dynamicEntities)
        {
            if (de == e)
            {
                continue;
            }
            
            float de_x = de->getPosition().x;
            float de_y = de->getPosition().y;
            float de_w = de->getWidth();
            float de_h = de->getHeight();
            Rektangle dynamicEntityBounds(de_x - de_w / 2, de_y - de_h / 2, de_w, de_h);
            
            if (OverlapTester::doRektanglesOverlap(playerBounds, dynamicEntityBounds))
            {
                if (playerBounds.right() >= dynamicEntityBounds.left() ||
                    playerBounds.left() <= dynamicEntityBounds.right() ||
                    playerBounds.top() >= dynamicEntityBounds.bottom() ||
                    playerBounds.bottom() <= dynamicEntityBounds.top())
                {
                    b2Vec2 vel = e->getVelocity();
                    vel *= FRAME_RATE;
                    b2Vec2 pos = e->getPosition();
                    pos -= vel;
                    e->setPosition(pos);
                }
                break;
            }
        }
    }
    
    for (Entity* e : _players)
    {
        // Enforce split screen bounds
        PlayerController* pc = static_cast<PlayerController*>(e->getController());
        uint8_t playerID = pc->getPlayerID();
        if (playerID == 1)
        {
            Rektangle play1ScreenBounds(0, 0, CFG_MAIN._splitScreenBarX, CFG_MAIN._camHeight);
            pc->enforceBounds(play1ScreenBounds);
        }
        else if (playerID == 2)
        {
            Rektangle play2ScreenBounds(CFG_MAIN._splitScreenBarX + CFG_MAIN._splitScreenBarWidth, 0, CFG_MAIN._splitScreenBarX, CFG_MAIN._camHeight);
            pc->enforceBounds(play2ScreenBounds);
        }
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
    return _map._key > 0;
}

std::string& World::getMapName()
{
    return _map._name;
}

std::string& World::getMapFileName()
{
    return _map._fileName;
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
