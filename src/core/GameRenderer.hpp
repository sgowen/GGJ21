//
//  GameRenderer.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

class Renderer;

class GameRenderer
{
public:
    void render(Renderer& r);
    
private:
    void renderWorld(Renderer& r);
    void renderEncounter(Renderer& r);
    void renderUI(Renderer& r);
};
