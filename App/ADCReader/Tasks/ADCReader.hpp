//
// Created by hubert25632 on 09.02.2022.
//

#ifndef F1XX_PROJECT_TEMPLATE_ADCREADER_HPP
#define F1XX_PROJECT_TEMPLATE_ADCREADER_HPP

#include "Tasks/UARTGatekeeper.hpp"

// This task reads analog voltage values on A3 and A4 pins and sends them on
// UART

class ADCReader : public SBT::System::PeriodicTask {
  // Pointer to the UART gatekeeper task
  std::shared_ptr<UARTGatekeeper> UARTGatekeeperTask;

public:
  // The constructor in this task takes a pointer to the UART gatekeeper task as
  // an argument
  ADCReader() = delete;
  ADCReader(std::shared_ptr<UARTGatekeeper> UARTGatekeeperTask);

  // initialize() will be called once when task is created
  void initialize() override;

  // run() will be called repeatedly
  void run() override;
};

#endif // F1XX_PROJECT_TEMPLATE_ADCREADER_HPP
