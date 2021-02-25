//
//  TitleRenderer.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "TitleRenderer.hpp"

#include "TitleEngineState.hpp"
#include "Renderer.hpp"
#include "TitleInputManager.hpp"

void TitleRenderer::render(Renderer& r)
{
    TitleEngineStateState tess = ENGINE_STATE_TITLE._state;
    
    r.bindFramebuffer();
    
    if (tess == TESS_START_DEDICATED_SERVER)
    {
        r.clearFramebuffer(Color::BLACK);
    }
    else
    {
        r.renderSprite("demo", "DEMO", 50, 60, 100, 80);
    }
    
    r.setTextVisible("dedicatedServerMode", tess == TESS_START_DEDICATED_SERVER);
    r.setTextVisible("hostServer", tess == TESS_DEFAULT);
    r.setTextVisible("joinServer", tess == TESS_DEFAULT);
    r.setTextVisible("enterIP", tess == TESS_INPUT_IP);
    r.setTextVisible("enterName", tess == TESS_INPUT_HOST_NAME || tess == TESS_INPUT_JOIN_NAME);
    r.setTextVisible("input", tess == TESS_INPUT_IP || tess == TESS_INPUT_HOST_NAME || tess == TESS_INPUT_JOIN_NAME);
    r.setText("input", INPUT_TITLE.getTextInput());
    r.setTextVisible("escToExit", tess != TESS_START_DEDICATED_SERVER);
    r.renderText();

    r.renderToScreen();
}
