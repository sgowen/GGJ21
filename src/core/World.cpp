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

#include "EntityMapper.hpp"
#include "EntityLayoutMapper.hpp"
#include "EntityNetworkController.hpp"
#include "InstanceManager.hpp"
#include "MainConfig.hpp"
#include "StringUtil.hpp"
#include "GowUtil.hpp"
#include "Network.hpp"
#include "Macros.hpp"
#include "PlayerController.hpp"
#include "OverlapTester.hpp"

World::World(TimeTracker* t, EntityIDManager* eidm, uint32_t flags) :
_timeTracker(t),
_entityIDManager(eidm),
_map(),
_flags(flags)
{
    // Empty
}

World::~World()
{
    clear();
}

void World::loadMap(uint32_t map)
{
    assert(map > 0);
    
    clear(IS_BIT_SET(_flags, WorldFlag_Client));
    
    _map._key = map;
    _map._name = StringUtil::stringFromFourChar(map);
    _map._fileName = ENTITY_LAYOUT_MAPPER.getJsonConfigFilePath(map);
    
    ENTITY_LAYOUT_MAPPER.loadEntityLayout(_map._key, _entityIDManager);
    
    EntityLayoutDef& eld = ENTITY_LAYOUT_MAPPER.getEntityLayoutDef();
    for (EntityInstanceDef eid : eld._entities)
    {
        EntityDef* ed = ENTITY_MAPPER.getEntityDef(eid._key);
        if (IS_BIT_SET(_flags, WorldFlag_Client) && IS_BIT_SET(ed->_bodyFlags, BODF_DYNAMIC))
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
        processPhysics(e);
        processCollisions(e, _staticEntities);
        processCollisions(e, _players);
        processCollisions(e, _dynamicEntities);
    }
    
    for (Entity* e : _players)
    {
        processPhysics(e);
        processCollisions(e, _staticEntities);
        processCollisions(e, _dynamicEntities);
    }
    
    for (Entity* e : _players)
    {
        // Enforce split screen bounds
        PlayerController* pc = static_cast<PlayerController*>(e->getController());
        uint8_t playerID = pc->getPlayerID();
        if (playerID == 1)
        {
            Rektangle play1ScreenBounds(0, 0, CFG_MAIN._splitScreenBarX, CFG_MAIN._camHeight);
            enforceBounds(e, play1ScreenBounds);
        }
        else if (playerID == 2)
        {
            Rektangle play2ScreenBounds(CFG_MAIN._splitScreenBarX + CFG_MAIN._splitScreenBarWidth, 0, CFG_MAIN._splitScreenBarX, CFG_MAIN._camHeight);
            enforceBounds(e, play2ScreenBounds);
        }
    }
}

void World::clear(bool skipDynamicEntities)
{
    GowUtil::cleanUpVectorOfPointers(_layers);
    GowUtil::cleanUpVectorOfPointers(_staticEntities);
    
    if (!skipDynamicEntities)
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
    return IS_BIT_SET(e->getEntityDef()._bodyFlags, BODF_STATIC);
}

bool World::isDynamic(Entity* e)
{
    return IS_BIT_SET(e->getEntityDef()._bodyFlags, BODF_DYNAMIC);
}

void World::refreshPlayers()
{
    _players.clear();
    
    for (Entity* e : _dynamicEntities)
    {
        if (e->getController()->getRTTI().derivesFrom(PlayerController::rtti))
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

void World::processPhysics(Entity* e)
{
    b2Vec2 vel = e->pose()._velocity;
    vel *= _timeTracker->_frameRate;
    e->pose()._position += vel;
}

void World::processCollisions(Entity* target, std::vector<Entity*>& entities)
{
    float x = target->getPosition().x;
    float y = target->getPosition().y;
    float w = target->getWidth();
    float h = target->getHeight();
    Rektangle bounds(x - w / 2, y - h / 2, w, h);
    
    for (Entity* e : entities)
    {
        if (target == e)
        {
            continue;
        }
        
        float x = e->getPosition().x;
        float y = e->getPosition().y;
        float w = e->getWidth();
        float h = e->getHeight();
        Rektangle boundsToTest(x - w / 2, y - h / 2, w, h);
        
        if (OverlapTester::doRektanglesOverlap(bounds, boundsToTest))
        {
            if (bounds.right() >= boundsToTest.left() ||
                bounds.left() <= boundsToTest.right() ||
                bounds.top() >= boundsToTest.bottom() ||
                bounds.bottom() <= boundsToTest.top())
            {
                b2Vec2 vel = target->getVelocity();
                vel *= _timeTracker->_frameRate;
                b2Vec2 pos = target->getPosition();
                pos -= vel;
                target->setPosition(pos);
            }
            
            target->getController()->onCollision(e);
            break;
        }
    }
}

void World::enforceBounds(Entity* e, Rektangle& bounds)
{
    float x = e->getPosition().x;
    float y = e->getPosition().y;
    
    if (x > bounds.right())
    {
        e->setPosition(b2Vec2(bounds.right(), y));
    }
    else if (x < bounds.left())
    {
        e->setPosition(b2Vec2(bounds.left(), y));
    }
    
    x = e->getPosition().x;
    if (y > bounds.top())
    {
        e->setPosition(b2Vec2(x, bounds.top()));
    }
    else if (y < bounds.bottom())
    {
        e->setPosition(b2Vec2(x, bounds.bottom()));
    }
}
