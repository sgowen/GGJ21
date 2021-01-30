//
//  MainRenderer.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "MainRenderer.hpp"

#include "MainEngineState.hpp"

#include "OpenGLWrapper.hpp"
#include "Macros.hpp"
#include "Color.hpp"
#include "MainConfig.hpp"
#include "InputManager.hpp"
#include "MainInputManager.hpp"
#include "StringUtil.hpp"
#include "FPSUtil.hpp"
#include "Constants.hpp"
#include "MainEngineState.hpp"

#include <sstream>
#include <ctime>
#include <string>
#include <assert.h>

MainRenderer::MainRenderer() :
_fontRenderer(128, 0, 0, 16, 64, 75, 1024, 1024),
_framebuffer(CFG_MAIN._framebufferWidth, CFG_MAIN._framebufferHeight),
_screenRenderer(),
_shaderManager(),
_spriteBatcher(128),
_textureManager(),
_textViews{
    TextView(TEXA_CENTER, "Hide"),
    TextView(TEXA_CENTER, "&"),
    TextView(TEXA_CENTER, "Jackie"),
    TextView(TEXA_CENTER, "[S]tart Server"),
    TextView(TEXA_CENTER, "[J]oin Server"),
    TextView(TEXA_CENTER, "Enter IP address", TEXV_HIDDEN),
    TextView(TEXA_CENTER, "Enter name", TEXV_HIDDEN),
    TextView(TEXA_CENTER, "", TEXV_HIDDEN),
    TextView(TEXA_LEFT,   "[ESC] to exit")
}
{
    _fontRenderer.setMatrixSize(CFG_MAIN._camWidth, CFG_MAIN._camHeight);
    _fontRenderer.configure(_textViews[0], 0.5f, 0.70f, 0.05f);
    _fontRenderer.configure(_textViews[1], 0.5f, 0.62f, 0.05f);
    _fontRenderer.configure(_textViews[2], 0.5f, 0.54f, 0.05f);
    _fontRenderer.configure(_textViews[3], 0.5f, 0.12f, 0.025f);
    _fontRenderer.configure(_textViews[4], 0.5f, 0.06f, 0.025f);
    _fontRenderer.configure(_textViews[5], 0.5f, 0.12f, 0.025f);
    _fontRenderer.configure(_textViews[6], 0.5f, 0.12f, 0.025f);
    _fontRenderer.configure(_textViews[7], 0.5f, 0.06f, 0.025f);
    _fontRenderer.configure(_textViews[8], 0.02f, 0.02f, 0.012f);
}

void MainRenderer::createDeviceDependentResources()
{
    _fontRenderer.createDeviceDependentResources();
    
    OGL.loadFramebuffer(_framebuffer);
    
    _screenRenderer.createDeviceDependentResources();
    _shaderManager.createDeviceDependentResources();
    _spriteBatcher.createDeviceDependentResources();
    _textureManager.createDeviceDependentResources();
}

void MainRenderer::onWindowSizeChanged(int screenWidth, int screenHeight)
{
    _screenRenderer.onWindowSizeChanged(screenWidth, screenHeight);
}

void MainRenderer::releaseDeviceDependentResources()
{
    _fontRenderer.releaseDeviceDependentResources();
    
    OGL.unloadFramebuffer(_framebuffer);
    
    _screenRenderer.releaseDeviceDependentResources();
    _shaderManager.releaseDeviceDependentResources();
    _spriteBatcher.releaseDeviceDependentResources();
    _textureManager.releaseDeviceDependentResources();
}

void MainRenderer::render()
{
    MainEngineStateState mess = ENGINE_STATE_MAIN._state;
    
    _textViews[3]._visibility = mess == MESS_DEFAULT ? TEXV_VISIBLE : TEXV_HIDDEN;
    _textViews[4]._visibility = mess == MESS_DEFAULT ? TEXV_VISIBLE : TEXV_HIDDEN;
    _textViews[5]._visibility = mess == MESS_INPUT_IP ? TEXV_VISIBLE : TEXV_HIDDEN;
    _textViews[6]._visibility = mess == MESS_INPUT_HOST_NAME || mess == MESS_INPUT_JOIN_NAME ? TEXV_VISIBLE : TEXV_HIDDEN;
    _textViews[7]._visibility = mess == MESS_INPUT_IP || mess == MESS_INPUT_HOST_NAME || mess == MESS_INPUT_JOIN_NAME ? TEXV_VISIBLE : TEXV_HIDDEN;
    _textViews[7]._text = INPUT_MAIN.getTextInput();
    
    int camWidth = CFG_MAIN._camWidth;
    int camHeight = CFG_MAIN._camHeight;
    
    updateMatrix(0, camWidth, 0, camHeight);
    
    OGL.bindFramebuffer(_framebuffer);
    OGL.enableBlending(true);
    
    _spriteBatcher.begin();
    TextureRegion demo(0, 0, 640, 480, 2048, 2048, 0);
    _spriteBatcher.addSprite(demo, camWidth / 2, camHeight / 2, camWidth, camHeight);
    _spriteBatcher.end(_shaderManager.shader("texture"), _matrix, _textureManager.texture("demo"));
    
    {
        Texture& t = _textureManager.texture("background_tiles");
        TextureDescriptor& td = t._descriptor;
        _spriteBatcher.begin();
        _spriteBatcher.addSprite(td.textureRegion("BG_BASEMENT_TILE_1"), camWidth / 2, camHeight * 0.4, 4, 4);
        _spriteBatcher.end(_shaderManager.shader("texture"), _matrix, t);
    }
    
    {
        static float hideFrameTime = 0;
        hideFrameTime++;
        Texture& t = _textureManager.texture("overworld_characters");
        TextureDescriptor& td = t._descriptor;
        _spriteBatcher.begin();
        _spriteBatcher.addSprite(td.textureRegion("HIDE_DOWN", hideFrameTime), camWidth / 2, camHeight * 0.4, 4, 4);
        _spriteBatcher.end(_shaderManager.shader("texture"), _matrix, t);
    }
    
    for (int i = 0; i < NUM_TEXT_VIEWS; ++i)
    {
        _fontRenderer.renderText(_shaderManager.shader("texture"), _textureManager.texture("texture_font"), _textViews[i]);
    }

    _screenRenderer.renderToScreen(_shaderManager.shader("framebuffer"), _framebuffer);
}

void MainRenderer::updateMatrix(float l, float r, float b, float t)
{
    mat4_identity(_matrix);
    mat4_ortho(_matrix, l, r, b, t, -1, 1);
    float w = r - l;
    float h = t - b;
    INPUT_MGR.setMatrixSize(w, h);
}
