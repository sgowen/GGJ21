//
//  JackiePlayerController.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/30/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "JackiePlayerController.hpp"

#include "Entity.hpp"
#include "CrystalController.hpp"
#include "GameInputState.hpp"
#include "Macros.hpp"
#include "GowAudioEngine.hpp"
#include "StringUtil.hpp"

IMPL_RTTI(JackiePlayerController, PlayerController)
IMPL_EntityController_create(JackiePlayerController)

std::string JackiePlayerController::getTextureMapping()
{
    switch (_stats._dir)
    {
        case PDIR_UP:
            return "JACKIE_UP";
        case PDIR_LEFT:
        case PDIR_RIGHT:
            return "JACKIE_LEFT";
        case PDIR_DOWN:
            return "JACKIE_DOWN";
        default:
            assert(false);
    }
}

void JackiePlayerController::onCollision(Entity* e)
{
    if (e->controller()->getRTTI().isDerivedFrom(CrystalController::rtti))
    {
        CrystalController* ec = static_cast<CrystalController*>(e->controller());
        
        ec->push(_stats._dir);
    }
}

bool ugh = false;
void JackiePlayerController::processInput(InputState* inputState, bool isLive)
{
    PlayerController::processInput(inputState, isLive);
    
    GameInputState* is = static_cast<GameInputState*>(inputState);
    GameInputState::PlayerInputState* pis = is->getPlayerInputStateForID(getPlayerID());
    if (pis == NULL)
    {
        return;
    }
    
    uint8_t piss = pis->inputState();
    if (IS_BIT_SET(piss, GISF_CONFIRM) && !ugh)
    {
        if (isLive)
        {
            ugh = true;
            LOG("play sound 1");
            GOW_AUDIO.playSound(_entity->getSoundMapping(1));
        }
    }
    else if (IS_BIT_SET(piss, GISF_CANCEL) && ugh)
    {
        if (isLive)
        {
            ugh = false;
            LOG("play sound 2");
            GOW_AUDIO.playSound(_entity->getSoundMapping(1));
        }
    }
}
