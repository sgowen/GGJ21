//
//  TitleRenderer.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "GGJ21.hpp"

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
        r.renderImageViews();
    }
    
    r.setTextVisible("dedicatedServerMode", tess == TESS_START_DEDICATED_SERVER);
    r.setTextVisible("hostServer", tess == TESS_DEFAULT);
    r.setTextVisible("joinServer", tess == TESS_DEFAULT);
    r.setTextVisible("enterIP", tess == TESS_INPUT_IP);
    r.setTextVisible("enterName", tess == TESS_INPUT_HOST_NAME || tess == TESS_INPUT_JOIN_NAME);
    r.setTextVisible("input", tess == TESS_INPUT_IP || tess == TESS_INPUT_HOST_NAME || tess == TESS_INPUT_JOIN_NAME);
    r.setText("input", INPUT_TITLE.getTextInput());
    r.setTextVisible("escToExit", tess != TESS_START_DEDICATED_SERVER);
    r.renderTextViews();

    r.renderToScreen();
}
