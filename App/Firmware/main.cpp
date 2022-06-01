#include "ReadTemp.hpp"
#include "SBT-SDK.hpp"
#include "TempCAN.hpp"

#ifdef SBT_TEMP_CANTEST
#include "CANPrint.hpp"
#endif

using namespace SBT::System;

// This is "main" - entry function that is called after system initialization
void entryPoint()
{
    Init();

    std::vector<std::shared_ptr<ReadTemp>> readTempTasks = {
        std::make_shared<ReadTemp>(&SBT::Hardware::uart2),
        std::make_shared<ReadTemp>(&SBT::Hardware::uart3)};

    for(const auto& readTempTask : readTempTasks)
        TaskManager::registerTask(readTempTask);

    TaskManager::registerTask(std::make_shared<TempCAN>(readTempTasks));

#ifdef SBT_TEMP_CANTEST
    TaskManager::registerTask(std::make_shared<CANPrint>());
#endif

    Start();
}
