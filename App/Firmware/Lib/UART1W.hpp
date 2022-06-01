//
// Created by hubert25632 on 07.05.22.
//

#ifndef TEMPERATURE_BOX_PROJECT_UART1W_HPP
#define TEMPERATURE_BOX_PROJECT_UART1W_HPP

#include "UART.hpp"

#include <FreeRTOS.h>
#include <semphr.h>

// 1-wire over UART driver. Requires the TX and RX lines to be connected using a
// resistor of (typical) value 5kΩ (or much less if parasite power scheme is to
// be used). TX line must operate in push-pull mode. All internal pull-ups and
// pull-downs must be disabled. RX line constitutes the 1-wire data bus.
class UART1W {
    SBT::Hardware::UART* const uart;
    SemaphoreHandle_t uartRxComplete;

    uint8_t txBuffer;
    uint8_t rxBuffer;

    void reinitUARTWithBaudrate(uint32_t baudrate);
    void uartRxCallback();

public:
    explicit UART1W(SBT::Hardware::UART* uart);

    /// Perform a bus reset
    /// \return true if any device has been detected, false otherwise
    bool Reset();

    /// Receive a single byte from the bus
    /// \return Received byte
    uint8_t ReceiveByte();

    /// Send a single byte to the bus
    /// \param byte Byte to send
    /// \return true if transmission was successful, false otherwise
    bool SendByte(uint8_t byte);
};

#endif // TEMPERATURE_BOX_PROJECT_UART1W_HPP
