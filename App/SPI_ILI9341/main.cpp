//
// Created by hubert25632 on 12.02.2022.
//

#include "SBT-SDK.hpp"
#include "Tasks/SPI_ILI9341.hpp"
#include "Tasks/UARTGatekeeper.hpp"

using namespace SBT::System;

// This is "main" - entry function that is called after system initialization
void entryPoint() {
  Init();

  auto UARTGatekeeperTask = std::make_shared<UARTGatekeeper>();
  TaskManager::registerTask(UARTGatekeeperTask);
  TaskManager::registerTask(std::make_shared<SPI_ILI9341>(UARTGatekeeperTask));

  Start();
}
