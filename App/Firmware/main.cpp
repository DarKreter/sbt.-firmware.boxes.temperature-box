#include "ExampleTask.hpp"
#include "SBT-SDK.hpp"

using namespace SBT::System;

// This is "main" - entry function that is called after system initialization
void entryPoint()
{
    Init();

    // Register your tasks here:
    TaskManager::registerTask(std::make_shared<ExampleTask>());

    Start();
}
