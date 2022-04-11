#include "SBT-SDK.hpp"
#include "Tasks/Sender.hpp"

using namespace SBT::System;

// This is "main" - entry function that is called after system initialization
void entryPoint() {
  Init();

  TaskManager::registerTask(std::make_shared<Sender>());

  Start();
}
