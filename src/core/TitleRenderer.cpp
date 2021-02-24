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
    
    r.textView("dedicatedServerMode")._visibility = tess == TESS_START_DEDICATED_SERVER;
    r.textView("hostServer")._visibility = tess == TESS_DEFAULT;
    r.textView("joinServer")._visibility = tess == TESS_DEFAULT;
    r.textView("enterIP")._visibility = tess == TESS_INPUT_IP;
    r.textView("enterName")._visibility = tess == TESS_INPUT_HOST_NAME || tess == TESS_INPUT_JOIN_NAME;
    r.textView("input")._visibility = tess == TESS_INPUT_IP || tess == TESS_INPUT_HOST_NAME || tess == TESS_INPUT_JOIN_NAME;
    r.textView("input")._text = INPUT_TITLE.getTextInput();
    r.textView("escToExit")._visibility = tess != TESS_START_DEDICATED_SERVER;
    r.renderText();

    r.renderToScreen();
}
