//
//  MainRenderer.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "MainRenderer.hpp"

#include "MainEngineState.hpp"

#include "OpenGLWrapper.hpp"
#include "Macros.hpp"
#include "Color.hpp"
#include "MainConfig.hpp"
#include "MainInputManager.hpp"
#include "CursorConverter.hpp"
#include "StringUtil.hpp"
#include "FPSUtil.hpp"

#include <sstream>
#include <ctime>
#include <string>
#include <assert.h>

MainRenderer::MainRenderer() :
_fontRenderer(128, 0, 0, 16, 64, 75, 1024, 1024),
_framebuffer(),
_screenRenderer(),
_shaderManager(),
_spriteBatcher(128),
_textureManager(),
_fpsTextView("", 98, 58, 2.24f, 2.625f, TextAlignment_RIGHT),
_messageTextView("Input anything to hear Jackie", 50, 6, 2.24f, 2.625f, TextAlignment_CENTER)
{
    // Empty
}

MainRenderer::~MainRenderer()
{
    // Empty
}

void MainRenderer::createDeviceDependentResources()
{
    _fontRenderer.createDeviceDependentResources();
    
    _framebuffer._width = CFG_MAIN._framebufferWidth;
    _framebuffer._height = CFG_MAIN._framebufferHeight;
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
    mat4_identity(_matrix);
    mat4_ortho(_matrix, 0, CFG_MAIN._camWidth, 0, CFG_MAIN._camHeight, -1, 1);
    CURSOR_CONVERTER.setMatrixSize(CFG_MAIN._camWidth, CFG_MAIN._camHeight);
    
    OGL.bindFramebuffer(_framebuffer);
    OGL.enableBlending(true);
    
    _spriteBatcher.begin();
    TextureRegion demo(0, 0, 640, 480, 2048, 2048, 0);
    _spriteBatcher.addSprite(demo, CFG_MAIN._camWidth / 2, CFG_MAIN._camHeight / 2, CFG_MAIN._camWidth, CFG_MAIN._camHeight);
    _spriteBatcher.end(_shaderManager.shader("shader_002"), _matrix, _textureManager.texture("demo"));
    
    int fps = FPS_UTIL.getFPS();
    _fpsTextView._text = StringUtil::format("%d FPS", fps);
    _fontRenderer.renderText(_shaderManager.shader("shader_002"), _matrix, _textureManager.texture("texture_font"), _fpsTextView);
    _fontRenderer.renderText(_shaderManager.shader("shader_002"), _matrix, _textureManager.texture("texture_font"), _messageTextView);

    _screenRenderer.renderToScreen(_shaderManager.shader("shader_001"), _framebuffer);
}
