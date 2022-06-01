//
// Created by hubert25632 on 07.05.22.
//

#include "ReadTemp.hpp"

ReadTemp::ReadTemp(SBT::Hardware::UART* uart)
    : SBT::System::PeriodicTask("ReadTemp", 4, 100, 128)
{
    thermometer = new DS18B20(uart);
    temperatureMailbox = xQueueCreate(1, sizeof(int16_t));
}

void ReadTemp::initialize() {}

void ReadTemp::run()
{
    auto temp = thermometer->GetTemperature();
    if(!temp)
        return;
    xQueueSend(temperatureMailbox, &temp.value(), 0);
}

std::optional<int16_t> ReadTemp::GetTemperature()
{
    int16_t temp;
    if(xQueueReceive(temperatureMailbox, &temp, 0) == pdTRUE)
        return temp;
    return std::nullopt;
}
