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
#include "HideController.hpp"
#include "MonsterController.hpp"
#include "CrystalController.hpp"
#include "EntityRenderController.hpp"
#include "ResourceManager.hpp"

#include <sstream>
#include <ctime>
#include <string>
#include <assert.h>

GameRenderer::GameRenderer() :
_renderer()
//_fontBatcher(256, 0, 0, 16, 16, 16, 256, 256),
//_framebuffer(CFG_MAIN._framebufferWidth, CFG_MAIN._framebufferHeight),
//_rektangleBatcher(2, true),
//_screenRenderer(),
//_spriteBatcher(4096),
//_textViews{
//    TextView(TEXA_CENTER, "Joining Server... [ESC] to exit", TEXV_HIDDEN),
//    TextView(TEXA_CENTER, "Lost Jackie again...", TEXV_HIDDEN),
//    TextView(TEXA_CENTER, "She needs to connect", TEXV_HIDDEN),
//    TextView(TEXA_CENTER, "a controller or", TEXV_HIDDEN),
//    TextView(TEXA_CENTER, "join me via IP.", TEXV_HIDDEN),
//    TextView(TEXA_CENTER, "Though I suppose", TEXV_HIDDEN),
//    TextView(TEXA_CENTER, "I could imagine her", TEXV_HIDDEN),
//    TextView(TEXA_CENTER, "with the arrow keys...", TEXV_HIDDEN),
//    TextView(TEXA_CENTER, "and unimagine her with .", TEXV_HIDDEN),
//    TextView(TEXA_CENTER, "", TEXV_HIDDEN),
//    TextView(TEXA_CENTER, "", TEXV_HIDDEN),
//    TextView(TEXA_CENTER, "", TEXV_HIDDEN),
//    TextView(TEXA_CENTER, "", TEXV_HIDDEN)
//}
{
//    _fontBatcher.setMatrixSize(CFG_MAIN._camWidth, CFG_MAIN._camHeight);
//    _fontBatcher.configure(_textViews[0], 0.5f, 0.12f, 0.02f);
//    _fontBatcher.configure(_textViews[1], 0.75f, 0.7f, 0.018f);
//    _fontBatcher.configure(_textViews[2], 0.75f, 0.6f, 0.018f);
//    _fontBatcher.configure(_textViews[3], 0.75f, 0.56f, 0.018f);
//    _fontBatcher.configure(_textViews[4], 0.75f, 0.52f, 0.018f);
//    _fontBatcher.configure(_textViews[5], 0.75f, 0.40f, 0.018f);
//    _fontBatcher.configure(_textViews[6], 0.75f, 0.36f, 0.018f);
//    _fontBatcher.configure(_textViews[7], 0.75f, 0.32f, 0.018f);
//    _fontBatcher.configure(_textViews[8], 0.75f, 0.28f, 0.018f);
//    _fontBatcher.configure(_textViews[9], 0.25f, 0.025f, 0.012f);
//    _fontBatcher.configure(_textViews[10], 0.75f, 0.025f, 0.012f);
}

void GameRenderer::createDeviceDependentResources()
{
    // TODO
}

void GameRenderer::onWindowSizeChanged(uint16_t screenWidth, uint16_t screenHeight)
{
    // TODO
}

void GameRenderer::releaseDeviceDependentResources()
{
    // TODO
}

void GameRenderer::render()
{
    updateMatrix(0, CFG_MAIN._camWidth, 0, CFG_MAIN._camHeight);

    OGL.bindFramebuffer(_framebuffer);
    OGL.enableBlending(true);
    
    renderWorld();
    renderEncounter();
    renderUI();
    
    renderToScreen("framebuffer", _framebuffer);

    _screenRenderer.renderToScreen(RES_MGR.shader("framebuffer"), _framebuffer);
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
    _spriteBatcher.end(RES_MGR.shader("texture"), _matrix, RES_MGR.texture("background_tiles"));
    
    _spriteBatcher.begin();
    addSpritesToBatcher(w.getPlayers());
    _spriteBatcher.end(RES_MGR.shader("texture"), _matrix, RES_MGR.texture("overworld_characters"));
}

void GameRenderer::renderEncounter()
{
    World& w = ENGINE_STATE_GAME_CLNT._world;
    if (!w.isEntityLayoutLoaded())
    {
        return;
    }
    
    bool isInEncounter = false;
    World& w = ENGINE_STATE_GAME_CLNT._world;
    Entity* hide = NULL;
    for (Entity* e : w.getPlayers())
    {
        if (e->controller()->getRTTI().isDerivedFrom(HideController::rtti))
        {
            hide = e;
            isInEncounter = e->controller<HideController>()->isInEncounter();
        }
    }
    
    if (!isInEncounter)
    {
        return;
    }
    
    _rektangleBatcher.begin();
    _rektangleBatcher.addRektangle(0,
                                 0,
                                 CFG_MAIN._camWidth / 2,
                                 CFG_MAIN._camHeight);
    _rektangleBatcher.end(RES_MGR.shader("geometry"), _matrix, Color::DIM);
    
    _spriteBatcher.begin();
    for (Entity* e : w.getNetworkEntities())
    {
        if (e->controller()->getRTTI().isDerivedFrom(MonsterController::rtti) &&
            e->controller<MonsterController>()->isInEncounter())
        {
            e->renderController<MonsterRenderController>()->addSpriteForEncounter(_spriteBatcher);
        }
    }
    
    hide->renderController<HideRenderController>()->addSpriteForEncounter(_spriteBatcher);
    
    _spriteBatcher.end(RES_MGR.shader("texture"), _matrix, RES_MGR.texture("big_sprites"));
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
        _rektangleBatcher.begin();
        for (Entity* e : w.getPlayers())
        {
            PlayerController* ec = e->controller<PlayerController>();
            
            uint8_t playerID = ec->getPlayerID();
            _textViews[playerID + 8]._visibility = TEXV_VISIBLE;
            
            if (playerID == 1)
            {
                _textViews[playerID + 8]._text = StringUtil::format("%s", ec->getUsername().c_str());
                _rektangleBatcher.addRektangle(0,
                                             0,
                                             CFG_MAIN._camWidth / 2,
                                             3);
            }
            else if (playerID == 2)
            {
                _textViews[playerID + 8]._text = StringUtil::format("%s @%s", ec->getUsername().c_str(), ec->getUserAddress().c_str());
                _rektangleBatcher.addRektangle(CFG_MAIN._camWidth / 2,
                                             0,
                                             CFG_MAIN._camWidth,
                                             3);
            }
        }
        _rektangleBatcher.end(RES_MGR.shader("geometry"), _matrix, Color::DIM);
    }
    
    if (NW_CLNT->state() == NWCS_WELCOMED)
    {
        if (w.getPlayers().size() == 1)
        {
            _rektangleBatcher.begin();
            _rektangleBatcher.addRektangle(CFG_MAIN._camWidth / 2,
                                         0,
                                         CFG_MAIN._camWidth,
                                         CFG_MAIN._camHeight);
            _rektangleBatcher.end(RES_MGR.shader("geometry"), _matrix, Color::BLACK);
            
            for (uint8_t i = 1; i <= 8; ++i)
            {
                _textViews[i]._visibility = TEXV_VISIBLE;
            }
        }
    }
    else
    {
        _textViews[0]._visibility = TEXV_VISIBLE;
    }
    
    _fontBatcher.begin();
    for (uint8_t i = 0; i < NUM_TEXT_VIEWS; ++i)
    {
        _fontBatcher.addText(_textViews[i]);
    }
    _fontBatcher.end(RES_MGR.shader("texture"), RES_MGR.texture("texture_font"));
}
