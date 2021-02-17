//
//  GameRenderer.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "FontRenderer.hpp"
#include "Framebuffer.hpp"
#include "ShaderInput.hpp"
#include "PolygonBatcher.hpp"
#include "ScreenRenderer.hpp"
#include "ShaderManager.hpp"
#include "SpriteBatcher.hpp"
#include "TextureManager.hpp"
#include "TextView.hpp"

#define NUM_TEXT_VIEWS 13

class Entity;

class GameRenderer
{
public:
    GameRenderer();
    ~GameRenderer() {}
    
    void createDeviceDependentResources();
    void onWindowSizeChanged(int screenWidth, int screenHeight);
    void releaseDeviceDependentResources();
    void render();
    
private:
    FontRenderer _fontRenderer;
    Framebuffer _framebuffer;
    mat4 _matrix;
    PolygonBatcher _polygonBatcher;
    ScreenRenderer _screenRenderer;
    ShaderManager _shaderManager;
    SpriteBatcher _spriteBatcher;
    TextureManager _textureManager;
    TextView _textViews[NUM_TEXT_VIEWS];
    
    void updateMatrix(float l, float r, float b, float t);
    void addSpritesToBatcher(std::vector<Entity*>& entities);
    
    void renderWorld();
    void renderEncounter();
    void renderUI();
    void renderSplitScreen();
};
