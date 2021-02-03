//
//  SoundUtil.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "SoundUtil.hpp"

#include "Entity.hpp"

#include "GowAudioEngine.hpp"

void SoundUtil::playSoundForStateIfChanged(Entity* e, uint8_t fromState, uint8_t toState)
{
    if (fromState == toState)
    {
        return;
    }
    
    GOW_AUDIO.playSound(e->getSoundMapping(toState));
}
