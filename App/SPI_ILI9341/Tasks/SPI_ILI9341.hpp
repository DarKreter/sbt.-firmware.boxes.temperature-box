//
// Created by hubert25632 on 09.02.2022.
//

#ifndef F1XX_PROJECT_TEMPLATE_SPI_ILI9341_HPP
#define F1XX_PROJECT_TEMPLATE_SPI_ILI9341_HPP

#include "SPI.hpp"
#include "Tasks/UARTGatekeeper.hpp"
#include <random>

// This task is designed to work with ILI9341-based LCD 240x320 display. The
// color depth is 18 bits per pixel (by default). Each pixel is defined by a
// 24-bit structure (8 bits per RGB channel). Two least significant bits of each
// channel are ignored by the controller.
// It is assumed that the ILI9341 signal pins are connected as follows:
// +──────────+────────+
// | ILI9341  | STM32  |
// +──────────+────────+
// | D/C      | B12    |
// | RESET    | B13    |
// | SDI      | MOSI   |
// | SDO      | MISO   |
// | SCK      | SCK    |
// | CS       | GND    |
// | LED      | 3V3    |
// +──────────+────────+
//
// This task generates 240x3 bytes of random data and sends it to the display
// controller. This algorithm is repeated 320 times in order to fill the whole
// screen with random pattern.

class SPI_ILI9341 : public SBT::System::PeriodicTask {
  // Pointer to the SPI device. It can be changed just by changing this
  // definition.
  SPI_t* const spi = &spi1;

  // Random Number Generator - it will be used to generate data which will be
  // sent to the display
  std::minstd_rand rng;

  // A union which enables conversion from 32-bit integers to 8-bit ones
  union {
    uint32_t u32_data[240 * 3 / 4];
    uint8_t u8_data[240 * 3];
  } random_data;

  // Transmission semaphore
  SemaphoreHandle_t TxComplete;

  // Pointer to the UART gatekeeper task
  const std::shared_ptr<UARTGatekeeper> UARTGatekeeperTask;

  // Used to run certain code only once in run()
  bool deviceReady = false;

  // Perform a controller reset
  void Reset();

  // Send command
  void SendCommand(uint8_t cmd);

  // Send data
  void SendData(uint8_t* data, size_t length);

public:
  // The constructor in this task takes a pointer to the UART gatekeeper task as
  // an argument
  SPI_ILI9341() = delete;
  SPI_ILI9341(std::shared_ptr<UARTGatekeeper> UARTGatekeeperTask);

  // initialize() will be called once when task is created
  void initialize() override;

  // run() will be called repeatedly
  void run() override;

  // A callback function
  void TxCallback();
};

#endif // F1XX_PROJECT_TEMPLATE_SPI_ILI9341_HPP
