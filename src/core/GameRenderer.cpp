//
//  GameRenderer.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GameRenderer.hpp"

#include "Renderer.hpp"
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

void GameRenderer::render(Renderer& r)
{
    r.bindFramebuffer();
    
    renderWorld(r);
    renderEncounter(r);
    renderUI(r);
    
    r.renderToScreen();
}

void GameRenderer::renderWorld(Renderer& r)
{
    World& w = ENGINE_STATE_GAME_CLNT._world;
    if (!w.isEntityLayoutLoaded())
    {
        return;
    }
    
    r.spriteBatcherBeginBatch();
    r.spriteBatcherAddEntities(w.getLayers());
    r.spriteBatcherAddEntities(w.getStaticEntities());
    r.spriteBatcherAddEntities(w.getNetworkEntities());
    r.spriteBatcherEndBatch("background_tiles");
    
    r.spriteBatcherBeginBatch();
    r.spriteBatcherAddEntities(w.getPlayers());
    r.spriteBatcherEndBatch("overworld_characters");
}

void GameRenderer::renderEncounter(Renderer& r)
{
    World& w = ENGINE_STATE_GAME_CLNT._world;
    if (!w.isEntityLayoutLoaded())
    {
        return;
    }
    
    bool isInEncounter = false;
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

void GameRenderer::renderUI(Renderer& r)
{
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
    
    r.renderText();
}
