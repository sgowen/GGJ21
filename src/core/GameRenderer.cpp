//
//  GameRenderer.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GameRenderer.hpp"

#include "Entity.hpp"
#include "GameClientEngineState.hpp"
#include "OpenGLWrapper.hpp"
#include "Macros.hpp"
#include "Color.hpp"
#include "MainConfig.hpp"
#include "InputManager.hpp"
#include "GameInputManager.hpp"
#include "StringUtil.hpp"
#include "FPSUtil.hpp"
#include "Assets.hpp"
#include "World.hpp"
#include "Network.hpp"
#include "NetworkClient.hpp"
#include "HidePlayerController.hpp"
#include "MonsterController.hpp"
#include "CrystalController.hpp"

#include <sstream>
#include <ctime>
#include <string>
#include <assert.h>

GameRenderer::GameRenderer() :
_fontRenderer(256, 0, 0, 16, 16, 16, 256, 256),
_framebuffer(CFG_MAIN._framebufferWidth, CFG_MAIN._framebufferHeight),
_polygonBatcher(2, true),
_screenRenderer(),
_shaderManager(),
_spriteBatcher(4096),
_textureManager(),
_textViews{
    TextView(TEXA_CENTER, "Joining Server... [ESC] to exit", TEXV_HIDDEN),
    TextView(TEXA_CENTER, "Lost Jackie again...", TEXV_HIDDEN),
    TextView(TEXA_CENTER, "She needs to connect", TEXV_HIDDEN),
    TextView(TEXA_CENTER, "a controller or", TEXV_HIDDEN),
    TextView(TEXA_CENTER, "join me via IP.", TEXV_HIDDEN),
    TextView(TEXA_CENTER, "Though I suppose", TEXV_HIDDEN),
    TextView(TEXA_CENTER, "I could imagine her", TEXV_HIDDEN),
    TextView(TEXA_CENTER, "with the arrow keys...", TEXV_HIDDEN),
    TextView(TEXA_CENTER, "and unimagine her with .", TEXV_HIDDEN),
    TextView(TEXA_CENTER, "", TEXV_HIDDEN),
    TextView(TEXA_CENTER, "", TEXV_HIDDEN),
    TextView(TEXA_CENTER, "", TEXV_HIDDEN),
    TextView(TEXA_CENTER, "", TEXV_HIDDEN)
}
{
    _fontRenderer.setMatrixSize(CFG_MAIN._camWidth, CFG_MAIN._camHeight);
    _fontRenderer.configure(_textViews[0], 0.5f, 0.12f, 0.02f);
    _fontRenderer.configure(_textViews[1], 0.75f, 0.7f, 0.018f);
    _fontRenderer.configure(_textViews[2], 0.75f, 0.6f, 0.018f);
    _fontRenderer.configure(_textViews[3], 0.75f, 0.56f, 0.018f);
    _fontRenderer.configure(_textViews[4], 0.75f, 0.52f, 0.018f);
    _fontRenderer.configure(_textViews[5], 0.75f, 0.40f, 0.018f);
    _fontRenderer.configure(_textViews[6], 0.75f, 0.36f, 0.018f);
    _fontRenderer.configure(_textViews[7], 0.75f, 0.32f, 0.018f);
    _fontRenderer.configure(_textViews[8], 0.75f, 0.28f, 0.018f);
    _fontRenderer.configure(_textViews[9], 0.24f, 0.025f, 0.02f);
    _fontRenderer.configure(_textViews[10], 0.76f, 0.025f, 0.02f);
}

void GameRenderer::createDeviceDependentResources()
{
    _fontRenderer.createDeviceDependentResources();
    
    OGL.loadFramebuffer(_framebuffer);
    
    _polygonBatcher.createDeviceDependentResources();
    _screenRenderer.createDeviceDependentResources();
    _shaderManager.createDeviceDependentResources();
    _spriteBatcher.createDeviceDependentResources();
    _textureManager.createDeviceDependentResources();
}

void GameRenderer::onWindowSizeChanged(int screenWidth, int screenHeight)
{
    _screenRenderer.onWindowSizeChanged(screenWidth, screenHeight);
}

void GameRenderer::releaseDeviceDependentResources()
{
    _fontRenderer.releaseDeviceDependentResources();
    
    OGL.unloadFramebuffer(_framebuffer);
    
    _polygonBatcher.releaseDeviceDependentResources();
    _screenRenderer.releaseDeviceDependentResources();
    _shaderManager.releaseDeviceDependentResources();
    _spriteBatcher.releaseDeviceDependentResources();
    _textureManager.releaseDeviceDependentResources();
}

void GameRenderer::render()
{
    updateMatrix(0, CFG_MAIN._camWidth, 0, CFG_MAIN._camHeight);

    OGL.bindFramebuffer(_framebuffer);
    OGL.enableBlending(true);
    
    renderWorld();
    renderEncounter();
    renderUI();
    renderSplitScreen();

    _screenRenderer.renderToScreen(_shaderManager.shader("framebuffer"), _framebuffer);
}

void GameRenderer::updateMatrix(float l, float r, float b, float t)
{
    mat4_identity(_matrix);
    mat4_ortho(_matrix, l, r, b, t, -1, 1);
    float w = r - l;
    float h = t - b;
    INPUT_MGR.setMatrixSize(w, h);
}

void GameRenderer::addSpritesToBatcher(std::vector<Entity*>& entities)
{
    for (Entity* e : entities)
    {
        TextureRegion tr = ASSETS.findTextureRegion(e->getTextureMapping(), e->stateTime());
        
        _spriteBatcher.addSprite(tr, e->getPosition()._x, e->getPosition()._y, e->width(), e->height(), e->getAngle(), e->isFacingLeft());
    }
}

void GameRenderer::renderWorld()
{
    World& w = ENGINE_STATE_GAME_CLNT._world;
    if (!w.isEntityLayoutLoaded())
    {
        return;
    }
    
    _spriteBatcher.begin();
    addSpritesToBatcher(w.getLayers());
    addSpritesToBatcher(w.getStaticEntities());
    addSpritesToBatcher(w.getNetworkEntities());
    _spriteBatcher.end(_shaderManager.shader("texture"), _matrix, _textureManager.texture("background_tiles"));
    
    _spriteBatcher.begin();
    addSpritesToBatcher(w.getPlayers());
    _spriteBatcher.end(_shaderManager.shader("texture"), _matrix, _textureManager.texture("overworld_characters"));
}

void GameRenderer::renderEncounter()
{
    bool isInEncounter = false;
    World& w = ENGINE_STATE_GAME_CLNT._world;
    for (Entity* e : w.getPlayers())
    {
        if (e->controller()->getRTTI().isDerivedFrom(HidePlayerController::rtti))
        {
            HidePlayerController* c = static_cast<HidePlayerController*>(e->controller());
            isInEncounter = c->isInEncounter();
        }
    }
    
    if (!isInEncounter)
    {
        return;
    }
    
    _polygonBatcher.begin();
    _polygonBatcher.addRektangle(0,
                                 0,
                                 CFG_MAIN._splitScreenBarX,
                                 CFG_MAIN._camHeight);
    _polygonBatcher.end(_shaderManager.shader("geometry"), _matrix, Color::DIM);
    
    _spriteBatcher.begin();
    for (Entity* e : w.getNetworkEntities())
    {
        if (e->controller()->getRTTI().isDerivedFrom(MonsterController::rtti))
        {
            MonsterController* ec = static_cast<MonsterController*>(e->controller());
            TextureRegion tr = ASSETS.findTextureRegion(ec->getTextureMappingForEncounter(), 0);
            _spriteBatcher.addSprite(tr, CFG_MAIN._monsterBattleX, CFG_MAIN._monsterBattleY, CFG_MAIN._monsterBattleWidth, CFG_MAIN._monsterBattleHeight, 0);
            break;
        }
    }
    for (Entity* e : w.getPlayers())
    {
        if (e->controller()->getRTTI().isDerivedFrom(HidePlayerController::rtti))
        {
            HidePlayerController* ec = static_cast<HidePlayerController*>(e->controller());
            TextureRegion tr = ASSETS.findTextureRegion(ec->getTextureMappingForEncounter(), ec->encounterStateTime());
            _spriteBatcher.addSprite(tr, CFG_MAIN._playerBattleX, CFG_MAIN._playerBattleY, ec->getWidthForEncounter(), CFG_MAIN._playerBattleHeight, 0);
            break;
        }
    }
    _spriteBatcher.end(_shaderManager.shader("texture"), _matrix, _textureManager.texture("big_sprites"));
}

void GameRenderer::renderUI()
{
    updateMatrix(0, CFG_MAIN._camWidth, 0, CFG_MAIN._camHeight);
    
    for (auto& tv : _textViews)
    {
        tv._visibility = TEXV_HIDDEN;
    }
    
    World& w = ENGINE_STATE_GAME_CLNT._world;
    if (CFG_MAIN._showDebug)
    {
        _polygonBatcher.begin();
        for (Entity* e : w.getPlayers())
        {
            PlayerController* c = static_cast<PlayerController*>(e->controller());
            
            int playerID = c->getPlayerID();
            _textViews[playerID + 8]._text = StringUtil::format("%s %s", c->getUsername().c_str(), c->getUserAddress().c_str());
            _textViews[playerID + 8]._visibility = TEXV_VISIBLE;
            
            if (playerID == 1)
            {
                _polygonBatcher.addRektangle(0,
                                             0,
                                             CFG_MAIN._splitScreenBarX,
                                             3);
            }
            else if (playerID == 2)
            {
                _polygonBatcher.addRektangle(CFG_MAIN._splitScreenBarX + CFG_MAIN._splitScreenBarWidth,
                                             0,
                                             CFG_MAIN._camWidth,
                                             3);
            }
        }
        _polygonBatcher.end(_shaderManager.shader("geometry"), _matrix, Color::DIM);
    }
    
    if (NW_CLNT != NULL &&
        NW_CLNT->state() == NWCS_WELCOMED)
    {
        if (w.getPlayers().size() == 1)
        {
            _polygonBatcher.begin();
            _polygonBatcher.addRektangle(CFG_MAIN._splitScreenBarX + CFG_MAIN._splitScreenBarWidth,
                                         CFG_MAIN._splitScreenBarY,
                                         CFG_MAIN._camWidth,
                                         CFG_MAIN._splitScreenBarY + CFG_MAIN._splitScreenBarHeight);
            _polygonBatcher.end(_shaderManager.shader("geometry"), _matrix, Color::BLACK);
            
            for (int i = 1; i <= 8; ++i)
            {
                _textViews[i]._visibility = TEXV_VISIBLE;
            }
        }
    }
    else
    {
        _textViews[0]._visibility = TEXV_VISIBLE;
    }
    
    for (int i = 0; i < NUM_TEXT_VIEWS; ++i)
    {
        _fontRenderer.renderText(_shaderManager.shader("texture"), _textureManager.texture("texture_font"), _textViews[i]);
    }
}

void GameRenderer::renderSplitScreen()
{
    _polygonBatcher.begin();
    _polygonBatcher.addRektangle(CFG_MAIN._splitScreenBarX,
                                 CFG_MAIN._splitScreenBarY,
                                 CFG_MAIN._splitScreenBarX + CFG_MAIN._splitScreenBarWidth,
                                 CFG_MAIN._splitScreenBarY + CFG_MAIN._splitScreenBarHeight);
    _polygonBatcher.end(_shaderManager.shader("geometry"), _matrix, Color::BLACK);
}
