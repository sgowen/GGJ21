//
//  SoundUtil.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/29/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <stdint.h>

class Entity;

class SoundUtil
{
public:
    static void playSoundForStateIfChanged(Entity* e, uint8_t fromState, uint8_t toState);
    
    SoundUtil() {}
    ~SoundUtil() {}
    SoundUtil(const SoundUtil&);
    SoundUtil& operator=(const SoundUtil&);
};
