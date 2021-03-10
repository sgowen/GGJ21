//
//  MonsterController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/v/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

IMPL_RTTI(MonsterController, EntityController)
IMPL_EntityController_create(MonsterController, EntityController)

MonsterController::MonsterController(Entity* e) : EntityController(e),
_battleAvatar(ENTITY_MGR.createEntity(EntityInstanceDef(0, 'MOA1', CFG_MAIN.monsterBattleX(), CFG_MAIN.monsterBattleY(), e->isServer())))
{
    // TODO, battle avatar position needs to be dynamic
    e->state()._stateFlags = EDIR_DOWN;
}

MonsterController::~MonsterController()
{
    delete _battleAvatar;
}

void MonsterController::update()
{
    if (_entity->dataField("isInBattle").valueBool())
    {
        // TODO
    }
    else
    {
        Vector2 playerPosition;
        uint8_t& state = _entity->state()._state;
        uint8_t& stateFlags = _entity->state()._stateFlags;
        state = STAT_IDLE;
        stateFlags = EDIR_DOWN;
        bool hasTarget = false;
        
        World& w = _entity->isServer() ? ENGINE_STATE_GAME_SRVR.world() : ENGINE_STATE_GAME_CLNT.world();
        std::vector<Entity*>& players = w.getPlayers();
        for (Entity* e : players)
        {
            uint8_t playerID = e->metadata().getUInt("playerID");
            if (playerID == 1)
            {
                float distance = e->position().dist(_entity->position());
                if (distance < CFG_MAIN.monsterLineOfSight())
                {
                    hasTarget = true;
                    playerPosition.set(e->position()._x, e->position()._y);
                }
            }
        }
        
        if (hasTarget)
        {
            float angle = playerPosition.sub(_entity->position()._x, _entity->position()._y).angle();
            float radians = DEGREES_TO_RADIANS(angle);
            _entity->pose()._velocity.set(cosf(radians) * CFG_MAIN.monsterMaxTopDownSpeed(), sinf(radians) * CFG_MAIN.monsterMaxTopDownSpeed());
            
            state = STAT_MOVING;
            
            if (_entity->pose()._velocity._y > 0)
            {
                stateFlags = EDIR_UP;
            }
        }
    }
}

void MonsterController::onCollision(Entity* e)
{
    if (e->controller()->getRTTI().isDerivedFrom(HideController::rtti))
    {
        e->message(MSG_ENCOUNTER);
        
        _entity->dataField("isInBattle").valueBool() = true;
        _entity->dataField("battleState").valueUInt8() = BSTT_IDLE;
        _entity->dataField("battleStateTime").valueUInt16() = 0;
        
        _entity->state()._state = MonsterController::STAT_IDLE;
        _entity->pose()._velocity.reset();
    }
}

Entity* MonsterController::battleAvatar()
{
    return _battleAvatar;
}
