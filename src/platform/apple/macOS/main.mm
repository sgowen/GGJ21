//
//  main.mm
//  GGJ21
//
//  Created by Stephen Gowen on 1/18/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include "MainEngineController.hpp"
#include "deps/glfw/GlfwMain.hpp"
#include "core/engine/AppleEngineControllerMacro.hpp"

int main(void)
{
    APPLE_INIT_BUNDLE_ROOT_FILE_PATH;
    MainEngineController controller(APPLE_BUNDLE_ROOT_FILE_PATH);
    return GlfwMain::exec(controller, "Lost Between Woods");
}
