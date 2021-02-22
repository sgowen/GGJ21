//
//  TitleRenderer.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "TitleRenderer.hpp"

#include "TitleEngineState.hpp"
#include "OpenGLWrapper.hpp"
#include "Macros.hpp"
#include "Color.hpp"
#include "MainConfig.hpp"
#include "InputManager.hpp"
#include "TitleInputManager.hpp"
#include "StringUtil.hpp"
#include "FPSUtil.hpp"
#include "ResourceManager.hpp"

#include <sstream>
#include <ctime>
#include <string>
#include <assert.h>

TitleRenderer::TitleRenderer() :
//_fontBatcher(256, 0, 0, 16, 16, 16, 256, 256),
//_framebuffer(CFG_MAIN._framebufferWidth, CFG_MAIN._framebufferHeight),
//_screenRenderer(),
//_spriteBatcher(128),
//_textViews{
//    TextView(TEXA_CENTER, "Dedicated Server Mode", TEXV_HIDDEN),
//    TextView(TEXA_CENTER, "[H]ost Server"),
//    TextView(TEXA_CENTER, "[J]oin Server"),
//    TextView(TEXA_CENTER, "Enter IP address", TEXV_HIDDEN),
//    TextView(TEXA_CENTER, "Enter name", TEXV_HIDDEN),
//    TextView(TEXA_CENTER, "", TEXV_HIDDEN),
//    TextView(TEXA_RIGHT,   "[ESC] to exit")
//}
{
//    _fontBatcher.setMatrixSize(CFG_MAIN._camWidth, CFG_MAIN._camHeight);
//    _fontBatcher.configure(_textViews[0], 0.5f, 0.50f, 0.025f);
//    _fontBatcher.configure(_textViews[3], 0.5f, 0.12f, 0.025f);
//    _fontBatcher.configure(_textViews[4], 0.5f, 0.06f, 0.025f);
//    _fontBatcher.configure(_textViews[5], 0.5f, 0.12f, 0.025f);
//    _fontBatcher.configure(_textViews[6], 0.5f, 0.12f, 0.025f);
//    _fontBatcher.configure(_textViews[7], 0.5f, 0.06f, 0.025f);
//    _fontBatcher.configure(_textViews[8], 0.98f, 0.02f, 0.012f);
}

void TitleRenderer::createDeviceDependentResources()
{
//    _fontBatcher.createDeviceDependentResources();
//    
//    OGL.loadFramebuffer(_framebuffer);
//    
//    _screenRenderer.createDeviceDependentResources();
//    _spriteBatcher.createDeviceDependentResources();
}

void TitleRenderer::onWindowSizeChanged(uint16_t screenWidth, uint16_t screenHeight)
{
//    _screenRenderer.onWindowSizeChanged(screenWidth, screenHeight);
}

void TitleRenderer::releaseDeviceDependentResources()
{
//    _fontBatcher.releaseDeviceDependentResources();
//
//    OGL.unloadFramebuffer(_framebuffer);
//
//    _screenRenderer.releaseDeviceDependentResources();
//    _spriteBatcher.releaseDeviceDependentResources();
}

void TitleRenderer::render()
{
    TitleEngineStateState mess = ENGINE_STATE_TITLE._state;
    
    _textViews[0]._visibility = mess == MESS_START_DEDICATED_SERVER ? TEXV_VISIBLE : TEXV_HIDDEN;
    _textViews[3]._visibility = mess == MESS_DEFAULT ? TEXV_VISIBLE : TEXV_HIDDEN;
    _textViews[4]._visibility = mess == MESS_DEFAULT ? TEXV_VISIBLE : TEXV_HIDDEN;
    _textViews[5]._visibility = mess == MESS_INPUT_IP ? TEXV_VISIBLE : TEXV_HIDDEN;
    _textViews[6]._visibility = mess == MESS_INPUT_HOST_NAME || mess == MESS_INPUT_JOIN_NAME ? TEXV_VISIBLE : TEXV_HIDDEN;
    _textViews[7]._visibility = mess == MESS_INPUT_IP || mess == MESS_INPUT_HOST_NAME || mess == MESS_INPUT_JOIN_NAME ? TEXV_VISIBLE : TEXV_HIDDEN;
    _textViews[7]._text = INPUT_MAIN.getTextInput();
    _textViews[8]._visibility = mess == MESS_START_DEDICATED_SERVER ? TEXV_HIDDEN : TEXV_VISIBLE;
    
    int camWidth = CFG_MAIN._camWidth;
    int camHeight = CFG_MAIN._camHeight;
    
    OGL.bindFramebuffer(_framebuffer);
    OGL.enableBlending(true);
    
    if (mess == MESS_START_DEDICATED_SERVER)
    {
        OGL.clearFramebuffer(COLOR_BLACK);
    }
    else
    {
        _spriteBatcher.begin();
        TextureRegion demo(0, 480, 1366, 619, 2048, 2048);
        _spriteBatcher.addSprite(demo, camWidth / 2, camHeight / 2 + (camHeight * 0.1f), camWidth, camHeight * 0.8f);
        _spriteBatcher.end(RES_MGR.shader("texture"), _matrix, RES_MGR.texture("demo"));
    }
    
    _fontBatcher.begin();
    for (int i = 0; i < NUM_TEXT_VIEWS; ++i)
    {
        _fontBatcher.addText(_textViews[i]);
    }
    _fontBatcher.end(RES_MGR.shader("texture"), RES_MGR.texture("texture_font"));

    _screenRenderer.renderToScreen(RES_MGR.shader("framebuffer"), _framebuffer);
}
