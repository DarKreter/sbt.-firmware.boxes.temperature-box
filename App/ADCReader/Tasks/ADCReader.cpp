//
// Created by hubert25632 on 09.02.2022.
//

#include "ADCReader.hpp"
#include "GPIO.hpp"

using namespace SBT::Hardware;

// Create a task named "ADCReader" with priority 3 and custom stack size 256.
// It will be executed every 0.8 seconds.
ADCReader::ADCReader(std::shared_ptr<UARTGatekeeper> UARTGatekeeperTask)
    : SBT::System::PeriodicTask("ADCReader", 3, 800, 256),
      UARTGatekeeperTask(std::move(UARTGatekeeperTask)) {}

void ADCReader::initialize() {
  // Enable A3 and A4 pins in analog input mode
  GPIO::Enable(GPIOA, GPIO_PIN_3, GPIO::Mode::AnalogInput, GPIO::Pull::NoPull);
  GPIO::Enable(GPIOA, GPIO_PIN_4, GPIO::Mode::AnalogInput, GPIO::Pull::NoPull);
}

void ADCReader::run() {
  // Read values and send a message
  UARTGatekeeperTask->SendString(new std::string(
      "ADC values:\r\n\tA3: " +
      std::to_string(GPIO::AnalogRead(GPIOA, GPIO_PIN_3)) + "\r\n\tA4: " +
      std::to_string(GPIO::AnalogRead(GPIOA, GPIO_PIN_4)) + "\r\n"));
}
