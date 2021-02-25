//
//  SoundUtil.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "SoundUtil.hpp"

#include "Entity.hpp"
#include "EntityRenderController.hpp"
#include "AudioEngineFactory.hpp"

void SoundUtil::playSoundForStateIfChanged(Entity* e, uint8_t fromState, uint8_t toState)
{
    if (fromState == toState)
    {
        return;
    }
    
    AUDIO_ENGINE.playSound(e->renderController()->getSoundMapping(toState));
}
