//
// Created by hubert25632 on 13.05.22.
//

#ifndef TEMPERATURE_BOX_PROJECT_CANPRINT_HPP
#define TEMPERATURE_BOX_PROJECT_CANPRINT_HPP

#include "CommCAN.hpp"
#include "TaskManager.hpp"
#include "UART.hpp"
#include <FreeRTOS.h>
#include <queue.h>

class CANPrint : public SBT::System::Task {
    SBT::Hardware::UART* const uart = &SBT::Hardware::uart1;
    QueueHandle_t messageQueue;

    void canCallback(SBT::System::Comm::CAN::RxMessage rxMessage);

public:
    CANPrint();
    void initialize() override;
    void run() override;
};

#endif // TEMPERATURE_BOX_PROJECT_CANPRINT_HPP
