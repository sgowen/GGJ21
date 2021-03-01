//
//  main.mm
//  GGJ21
//
//  Created by Stephen Gowen on 1/18/21.
//  Copyright © 2021 Stephen Gowen. All rights reserved.
//

#include <GowEngine/GowEngine.hpp>
#include <GowEngine/AppleMacros.hpp>
#include "GGJ21.hpp"

int main(void)
{
    APPLE_INIT_BUNDLE_ROOT_FILE_PATH;
    MainEngineController controller(APPLE_BUNDLE_ROOT_FILE_PATH);
    return GlfwMain::exec(controller, "Lost Between Woods");
}
