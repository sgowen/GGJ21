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
    static void render(Renderer& r);
    
private:
    static void renderWorld(Renderer& r);
    static void renderEncounter(Renderer& r);
    static void renderUI(Renderer& r);
    
    GameRenderer() {}
    ~GameRenderer() {}
    GameRenderer(const GameRenderer&);
    GameRenderer& operator=(const GameRenderer&);
};
