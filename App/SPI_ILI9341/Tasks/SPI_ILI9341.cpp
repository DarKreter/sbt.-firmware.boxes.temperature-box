//
// Created by hubert25632 on 09.02.2022.
//

#include "SPI_ILI9341.hpp"
#include "GPIO.hpp"

// Pin definitions
#define ILI9341_DC_PORT    GPIOB
#define ILI9341_DC_PIN     GPIO_PIN_12
#define ILI9341_RESET_PORT GPIOB
#define ILI9341_RESET_PIN  GPIO_PIN_13

// ILI9341 commands used in this example
#define ILI9341_CMD_SLEEP_OUT    0x11
#define ILI9341_CMD_DISPLAY_ON   0x29
#define ILI9341_CMD_MEMORY_WRITE 0x2c

using namespace SBT::Hardware;

// Create a task named "SPI_ILI9341" with priority 3. It will be executed every
// 0.5 seconds.
SPI_ILI9341::SPI_ILI9341(std::shared_ptr<UARTGatekeeper> UARTGatekeeperTask)
    : SBT::System::PeriodicTask("SPI_ILI9341", 3, 500, 128),
      UARTGatekeeperTask(std::move(UARTGatekeeperTask)) {
  // Create a binary semaphore
  TxComplete = xSemaphoreCreateBinary();
}

void SPI_ILI9341::initialize() {
  // Use SPI in DMA mode
  spi->ChangeModeToDMA();

  // Some parameters of SPI can be changed before initialization using Set*()
  // functions (such as SetBaudRate()). See SPI.hpp for more details.
  spi->SetClockPolarity(SPI_t::ClockPolarity::LOW);
  spi->SetClockPhase(SPI_t::ClockPhase::_1EDGE);
  spi->SetBaudRate(9e6);

  // Initialize the driver
  spi->Initialize();

  // Register interrupt callback function. It will be called when a specific
  // event such as successful transmission or reception occurs.
  spi->RegisterCallback(SPI_t::CallbackType::TxComplete, this,
                        &SPI_ILI9341::TxCallback);

  // Enable GPIO ports to serve as DC and RESET pins
  GPIO::Enable(ILI9341_DC_PORT, ILI9341_DC_PIN, GPIO::Mode::Output,
               GPIO::Pull::NoPull);
  GPIO::Enable(ILI9341_RESET_PORT, ILI9341_RESET_PIN, GPIO::Mode::Output,
               GPIO::Pull::NoPull);
}

void SPI_ILI9341::run() {

  // Set up the display controller
  if (!deviceReady) {
    // Perform a controller reset
    Reset();

    // The controller is in Sleep In mode after reset. Sleep Out command must be
    // executed before it is ready to accept further commands.
    SendCommand(ILI9341_CMD_SLEEP_OUT);

    // It takes at most 5ms to enter Sleep Out mode.
    vTaskDelay(5);

    // Enable the display. It is not necessary to wait after issuing this
    // command.
    SendCommand(ILI9341_CMD_DISPLAY_ON);

    // The device is now ready
    deviceReady = true;
  }

  // Send Memory Write command
  SendCommand(ILI9341_CMD_MEMORY_WRITE);

  // Write 320 lines of data
  for (unsigned i = 0; i < 320; i++) {
    // Generate random data for each line
    for (unsigned j = 0; j < 240 * 3 / 4; j++)
      random_data.u32_data[j] = rng();

    // Send random data to the display controller
    SendData(random_data.u8_data, 240 * 3);
  }

  // Send a TX complete message
  UARTGatekeeperTask->SendString(new std::string("SPI TX complete\r\n"));
}

void SPI_ILI9341::TxCallback() {
  BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

  // Give to a TxComplete semaphore unblocking the task waiting for it
  xSemaphoreGiveFromISR(TxComplete, &pxHigherPriorityTaskWoken);

  // Perform a context switch if necessary
  portYIELD_FROM_ISR(pxHigherPriorityTaskWoken)
}

void SPI_ILI9341::Reset() {
  // Drive the RESET line low to start a reset pulse
  GPIO::DigitalWrite(ILI9341_RESET_PORT, ILI9341_RESET_PIN, GPIO::State::LOW);

  // The RESET line must be held low for at least 10μs. Such precision is not
  // attainable by software, so we wait 1ms.
  vTaskDelay(1);

  // Drive the RESET line high to start a reset sequence
  GPIO::DigitalWrite(ILI9341_RESET_PORT, ILI9341_RESET_PIN, GPIO::State::HIGH);

  // It takes at most 120ms to perform a controller reset
  vTaskDelay(120);
}

void SPI_ILI9341::SendCommand(uint8_t cmd) {
  // Drive the DC line low to send a command
  GPIO::DigitalWrite(ILI9341_DC_PORT, ILI9341_DC_PIN, GPIO::State::LOW);

  // Send the command
  spi->Send(&cmd, 1);

  // Block indefinitely until the transmission is complete
  xSemaphoreTake(TxComplete, portMAX_DELAY);
}

void SPI_ILI9341::SendData(uint8_t* data, size_t length) {
  // Drive the DC line high to send data
  GPIO::DigitalWrite(ILI9341_DC_PORT, ILI9341_DC_PIN, GPIO::State::HIGH);

  // Send data
  spi->Send(data, length);

  // Drive the DC line low to send a command
  xSemaphoreTake(TxComplete, portMAX_DELAY);
}
