//
// Created by hubert25632 on 09.02.2022.
//

#include "I2C_24AA01.hpp"
#include "Libs/byte2hex.hpp"

// 24AA01 EEPROM I2C address
#define I2C_24AA01_ADDR 0xa0

// Create a task named "I2C_24AA01" with priority 3 and custom stack size 256.
// It will be executed every 5 seconds.
I2C_24AA01::I2C_24AA01(std::shared_ptr<UARTGatekeeper> UARTGatekeeperTask)
    : SBT::System::PeriodicTask("I2C_24AA01", 3, 5000, 256),
      UARTGatekeeperTask(std::move(UARTGatekeeperTask)) {
  // Create binary semaphores
  TxComplete = xSemaphoreCreateBinary();
  RxComplete = xSemaphoreCreateBinary();
}

void I2C_24AA01::initialize() {
  // Use I2C in DMA mode
  i2c->ChangeModeToDMA();

  // Some parameters of I2C can be changed before initialization using Set*()
  // functions (such as SetSpeed()). See I2C.hpp for more details.

  // Initialize the driver. Own address is irrelevant in master mode.
  i2c->Initialize(0);

  // Register interrupt callback functions. They will be called when a specific
  // event such as successful transmission or reception occurs.
  i2c->RegisterCallback(I2C::CallbackType::MemTxComplete, this,
                        &I2C_24AA01::TxCallback);
  i2c->RegisterCallback(I2C::CallbackType::MemRxComplete, this,
                        &I2C_24AA01::RxCallback);
}

void I2C_24AA01::run() {
  // Generate 32 4-byte random numbers
  for (uint32_t& i : random_data.u32_data)
    i = rng();

  // Write 128 bytes of data in groups of 8 - it is the size of the 24AA01 page
  // buffer
  for (unsigned i = 0; i < 128; i += 8) {
    // Write 8 bytes
    i2c->WriteRegister(I2C_24AA01_ADDR, i, 1, random_data.u8_data + i, 8);

    // Block indefinitely until the transmission is complete
    xSemaphoreTake(TxComplete, portMAX_DELAY);

    // Wait 5ms so that the EEPROM can complete its write cycle
    vTaskDelay(5);
  }

  // Send a TX complete message
  UARTGatekeeperTask->SendString(new std::string("EEPROM TX complete\r\n"));

  // Receive 128 bytes
  i2c->ReadRegister(I2C_24AA01_ADDR, 0, 1, received_data, 128);

  // Block indefinitely until the reception is complete
  xSemaphoreTake(RxComplete, portMAX_DELAY);

  // Send an RX complete message
  UARTGatekeeperTask->SendString(new std::string("EEPROM RX complete\r\n"));

  // Compare generated and received data byte-by-byte
  for (unsigned i = 0; i < 128; i++) {
    // If there is a difference, send a message
    if (received_data[i] != random_data.u8_data[i])
      UARTGatekeeperTask->SendString(
          new std::string("Difference on byte " + std::to_string(i) + ": 0x" +
                          byte2hex(random_data.u8_data[i]) + " -> 0x" +
                          byte2hex(received_data[i]) + "\r\n"));
  }
}

void I2C_24AA01::TxCallback() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  // Give to a TxComplete semaphore unblocking the task waiting for it
  xSemaphoreGiveFromISR(TxComplete, &xHigherPriorityTaskWoken);

  // Perform a context switch if necessary
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken)
}

void I2C_24AA01::RxCallback() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  // Give to a RxComplete semaphore unblocking the task waiting for it
  xSemaphoreGiveFromISR(RxComplete, &xHigherPriorityTaskWoken);

  // Perform a context switch if necessary
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken)
}
