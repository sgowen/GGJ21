//
//  TitleRenderer.h
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include "FontRenderer.hpp"
#include "Framebuffer.hpp"
#include "ShaderInput.hpp"
#include "ScreenRenderer.hpp"
#include "ShaderManager.hpp"
#include "SpriteBatcher.hpp"
#include "TextureManager.hpp"
#include "TextView.hpp"

#define NUM_TEXT_VIEWS 9

class TitleRenderer
{
public:
    TitleRenderer();
    ~TitleRenderer() {}
    
    void createDeviceDependentResources();
    void onWindowSizeChanged(int screenWidth, int screenHeight);
    void releaseDeviceDependentResources();
    void render();
    
private:
    FontRenderer _fontRenderer;
    Framebuffer _framebuffer;
    mat4 _matrix;
    ScreenRenderer _screenRenderer;
    ShaderManager _shaderManager;
    SpriteBatcher _spriteBatcher;
    TextureManager _textureManager;
    TextView _textViews[NUM_TEXT_VIEWS];
    
    void updateMatrix(float l, float r, float b, float t);
};
