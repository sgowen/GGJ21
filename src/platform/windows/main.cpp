//
//  main.cpp
//  GGJ21
//
//  Created by Stephen Gowen on 1/18/21.
//  Copyright � 2020 Stephen Gowen. All rights reserved.
//

#include "MainEngineController.hpp"
#include "glfw/GlfwMain.hpp"

int main(void)
{
    MainEngineController controller;
    return GlfwMain::exec(controller, "Lost Between Woods");
}
