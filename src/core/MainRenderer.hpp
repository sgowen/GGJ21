//
//  MainRenderer.h
//  GGJ21
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <string>

#include "FontRenderer.hpp"
#include "Framebuffer.hpp"
#include "ShaderInput.hpp"
#include "ScreenRenderer.hpp"
#include "ShaderManager.hpp"
#include "SpriteBatcher.hpp"
#include "TextureManager.hpp"
#include "TextView.hpp"

class MainRenderer
{
public:
    MainRenderer();
    ~MainRenderer();
    
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
    TextView _fpsTextView;
    TextView _messageTextView;
};
