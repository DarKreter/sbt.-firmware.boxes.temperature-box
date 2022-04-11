//
// Created by hubert25632 on 09.02.2022.
//

#ifndef F1XX_PROJECT_TEMPLATE_I2C_24AA01_HPP
#define F1XX_PROJECT_TEMPLATE_I2C_24AA01_HPP

#include "I2C.hpp"
#include "Tasks/UARTGatekeeper.hpp"
#include <random>

// This task is designed to work with 24AA01 EEPROM chip (128 bytes). It must be
// connected for this task not to hang up.
// This task generates 128 bytes of random data and sends it to the EEPROM. Then
// it reads 128 bytes from the chip and compares both sets of data. Upon finding
// a difference it sends an appropriate message on UART.

class I2C_24AA01 : public SBT::System::PeriodicTask {
  // Pointer to the I2C device. It can be changed just by changing this
  // definition.
  I2C* const i2c = &i2c1;

  // Random Number Generator - it will be used to generate data which will be
  // sent to the EEPROM
  std::minstd_rand rng;

  // A union which enables conversion from 32-bit integers to 8-bit ones
  union {
    uint32_t u32_data[32];
    uint8_t u8_data[128];
  } random_data;

  // Received data buffer
  uint8_t received_data[128];

  // Transmission and reception semaphores
  SemaphoreHandle_t TxComplete, RxComplete;

  // Pointer to the UART gatekeeper task
  const std::shared_ptr<UARTGatekeeper> UARTGatekeeperTask;

public:
  // The constructor in this task takes a pointer to the UART gatekeeper task as
  // an argument
  I2C_24AA01() = delete;
  I2C_24AA01(std::shared_ptr<UARTGatekeeper> UARTGatekeeperTask);

  // initialize() will be called once when task is created
  void initialize() override;

  // run() will be called repeatedly
  void run() override;

  // Callback functions
  void TxCallback();
  void RxCallback();
};

#endif // F1XX_PROJECT_TEMPLATE_I2C_24AA01_HPP
