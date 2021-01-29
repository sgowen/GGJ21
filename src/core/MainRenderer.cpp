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

#include <sstream>
#include <ctime>
#include <string>
#include <assert.h>

MainRenderer::MainRenderer() :
_font(0, 0, 16, 64, 75, 1024, 1024),
_framebuffer(2048, 2048),
_screenRenderer(),
_shaderManager(),
_spriteBatcher(128),
_textureManager()
{
    // Empty
}

MainRenderer::~MainRenderer()
{
    // Empty
}

void MainRenderer::createDeviceDependentResources()
{
    OGL.loadFramebuffer(_framebuffer, GL_LINEAR, GL_LINEAR);
    
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
    
    // TODO
    
    _spriteBatcher.end(_shaderManager.shader("shader_002"), _matrix, _textureManager.texture("texture_font"));

    _screenRenderer.renderToScreen(_shaderManager.shader("shader_001"), _framebuffer);
}
