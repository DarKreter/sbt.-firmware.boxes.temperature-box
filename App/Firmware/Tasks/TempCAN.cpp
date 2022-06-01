//
// Created by hubert25632 on 13.05.22.
//

#include "TempCAN.hpp"

using namespace SBT::System::Comm;

TempCAN::TempCAN(std::vector<std::shared_ptr<ReadTemp>> readTempTasks)
    : SBT::System::PeriodicTask("TempCAN", 3, 1000, 128),
      readTempTasks(std::move(readTempTasks))
{
}

void TempCAN::initialize() {}

void TempCAN::run()
{
    auto temp1 = readTempTasks[0]->GetTemperature();
    auto temp2 = readTempTasks[1]->GetTemperature();

    if(!(temp1 || temp2))
        return;

    temperaturePowerbox.temperature1 = temp1.value_or(INT16_MIN);
    temperaturePowerbox.temperature2 = temp2.value_or(INT16_MIN);

    Pack_TEMPERATURE_POWERBOX(&temperaturePowerbox, canPayload);
    CAN::Send(CAN_ID::Message::TEMPERATURE_POWERBOX, canPayload);
}
