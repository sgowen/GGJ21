//
//  TitleRenderer.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "Renderer.hpp"

#include <stdint.h>

class TitleRenderer
{
public:
    TitleRenderer();
    
    void createDeviceDependentResources();
    void onWindowSizeChanged(uint16_t screenWidth, uint16_t screenHeight);
    void releaseDeviceDependentResources();
    void render();
    
private:
    Renderer _renderer;
};
