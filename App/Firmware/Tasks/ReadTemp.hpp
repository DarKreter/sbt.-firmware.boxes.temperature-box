//
// Created by hubert25632 on 07.05.22.
//

#ifndef TEMPERATURE_BOX_PROJECT_READTEMP_HPP
#define TEMPERATURE_BOX_PROJECT_READTEMP_HPP

#include "DS18B20.hpp"
#include "TaskManager.hpp"

class ReadTemp : public SBT::System::PeriodicTask {
    DS18B20* thermometer;
    QueueHandle_t temperatureMailbox;

public:
    explicit ReadTemp(SBT::Hardware::UART* uart);
    void initialize() override;
    void run() override;

    std::optional<int16_t> GetTemperature();
};

#endif // TEMPERATURE_BOX_PROJECT_READTEMP_HPP
