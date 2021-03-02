//
//  main.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/18/21.
//  Copyright © 2020 Stephen Gowen. All rights reserved.
//

#include <GowEngine/GowEngine.hpp>
#include "GGJ21.hpp"

int main(void)
{
    MainEngineController controller;
    return GlfwMain::exec(controller, "Lost Between Woods");
}
