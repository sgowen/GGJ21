//
//  GameRenderer.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

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
    
    r.spriteBatcherBegin();
    r.spriteBatcherAddEntities(w.getLayers());
    r.spriteBatcherAddEntities(w.getStaticEntities());
    r.spriteBatcherAddEntities(w.getNetworkEntities());
    r.spriteBatcherEnd("background_tiles");
    
    r.spriteBatcherBegin();
    r.spriteBatcherAddEntities(w.getPlayers());
    r.spriteBatcherEnd("overworld_characters");
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
    
    Matrix& m = r.matrix();
    float width = m._desc.width();
    float height = m._desc.height();
    
    r.rektangleBatcherBegin();
    Rektangle encounterBackground(0, 0, width / 2, height);
    r.rektangleBatcherAddRektangle(encounterBackground);
    r.rektangleBatcherEnd(Color::DIM);
    
    r.spriteBatcherBegin();
    for (Entity* e : w.getNetworkEntities())
    {
        if (e->controller()->getRTTI().isDerivedFrom(MonsterController::rtti) &&
            e->controller<MonsterController>()->isInEncounter())
        {
            e->renderController<MonsterRenderController>()->addSpriteForEncounter(r.spriteBatcher());
        }
    }
    hide->renderController<HideRenderController>()->addSpriteForEncounter(r.spriteBatcher());
    r.spriteBatcherEnd("big_sprites");
}

void GameRenderer::renderUI(Renderer& r)
{
    r.hideAllText();
    
    Matrix& m = r.matrix();
    float width = m._desc.width();
    float height = m._desc.height();
    
    World& w = ENGINE_STATE_GAME_CLNT._world;
    if (CFG_MAIN.showDebug())
    {
        r.rektangleBatcherBegin();
        for (Entity* e : w.getPlayers())
        {
            PlayerController* ec = e->controller<PlayerController>();
            
            uint8_t playerID = e->entityDef()._data.getUInt("playerID");
            if (playerID == 1)
            {
                r.setTextVisible("player1Info", true);
                r.setText("player1Info", StringUtil::format("%s", ec->getUsername().c_str()));
                Rektangle player1InfoBar(0, 0, width / 2, 3);
                r.rektangleBatcherAddRektangle(player1InfoBar);
            }
            else if (playerID == 2)
            {
                r.setTextVisible("player2Info", true);
                r.setText("player2Info", StringUtil::format("%s @%s", ec->getUsername().c_str(), ec->getUserAddress().c_str()));
                Rektangle player2InfoBar(width / 2, 0, width, 3);
                r.rektangleBatcherAddRektangle(player2InfoBar);
            }
        }
        r.rektangleBatcherEnd(Color::DIM);
    }
    
    if (NW_CLNT->state() == NWCS_WELCOMED)
    {
        if (w.getPlayers().size() == 1)
        {
            r.rektangleBatcherBegin();
            Rektangle player2BlackedOut(width / 2, 0, width, height);
            r.rektangleBatcherAddRektangle(player2BlackedOut);
            r.rektangleBatcherEnd(Color::BLACK);
            
#if IS_MOBILE
            r.setTextVisible("lostJackieAgain_mobile", true);
#else
            r.setTextVisible("lostJackieAgain", true);
#endif
        }
    }
    else
    {
        r.setTextVisible("joiningServer", true);
    }
    
    r.renderTextViews();
    
    MoveList& ml = INPUT_GAME.moveList();
    if (ml.getMoveCount() == NW_CLNT_MAX_NUM_MOVES)
    {
        Matrix& m = r.matrix();
        float width = m._desc.width();
        float height = m._desc.height();
        
        Rektangle networkLagBg(0, 0, width, height);
        Color networkLag(1, 0, 0, 0.1);
        r.rektangleBatcherBegin();
        r.rektangleBatcherAddRektangle(networkLagBg);
        r.rektangleBatcherEnd(networkLag);
    }
}
