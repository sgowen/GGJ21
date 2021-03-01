//
//  TitleRenderer.hpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/27/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#pragma once

#include <GowEngine/GowEngine.hpp>

class TitleRenderer
{
public:
    static void render(Renderer& r);
    
private:
    TitleRenderer() {}
    ~TitleRenderer() {}
    TitleRenderer(const TitleRenderer&);
    TitleRenderer& operator=(const TitleRenderer&);
};
