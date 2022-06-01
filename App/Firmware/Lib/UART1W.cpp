//
// Created by hubert25632 on 07.05.22.
//

#include "UART1W.hpp"

constexpr TickType_t UART1W_max_rx_delay_ms = 10;

constexpr uint32_t UART1W_reset_rate = 9600;
constexpr uint32_t UART1W_tx_rx_rate = 115200;

constexpr uint8_t UART1W_reset_byte = 0xf0;
constexpr uint8_t UART1W_rx_byte = 0xff;
constexpr uint8_t UART1W_tx_0_byte = 0x00;
constexpr uint8_t UART1W_tx_1_byte = 0xff;
// An arbitrary value different from 0x00 and 0xff.
constexpr uint8_t UART1W_tx_dummy = 0xa5;

using namespace SBT::Hardware;

UART1W::UART1W(UART* uart) : uart(uart)
{
    uart->ChangeModeToDMA();
    uart->Initialize();
    uart->RegisterCallback(UART::CallbackType::RxComplete, this,
                           &UART1W::uartRxCallback);

    uartRxComplete = xSemaphoreCreateBinary();
}

// 1-wire bus reset is performed by pulling the data bus low for at least 480μs.
// In order to accomplish this, the UART rate is set to 9600 baud. This causes
// one bit to be approximately 104.17μs long (1/9600s). Sending the 0xf0 byte
// causes the bus to be pulled low for the duration of 5 bits (start symbol + 4
// '0' bits -- note that bits in UART are sent LSB first) which is approximately
// 520.83μs. The rest of the byte (4 '1' bits) is used for presence detection.
// If there is any device present, it waits for at most 60μs, and then it pulls
// the bus low for at least 60μs, which is enough to be detected as at least one
// '0' bit. This causes the received byte to differ from the sent one. If the
// received and sent bytes are the same, it means that there are no devices on
// the bus. Being unable to receive any byte for more than
// UART1W_max_rx_delay_ms is treated as if there was no device present (this can
// happen if the TX and RX lines are not connected).
bool UART1W::Reset()
{
    reinitUARTWithBaudrate(UART1W_reset_rate);

    rxBuffer = UART1W_reset_byte;
    uart->Receive(&rxBuffer, 1);

    txBuffer = UART1W_reset_byte;
    uart->Send(&txBuffer, 1);

    if(xSemaphoreTake(uartRxComplete, UART1W_max_rx_delay_ms) == pdFALSE)
        uart->AbortRx();

    return rxBuffer != txBuffer;
}

uint8_t UART1W::ReceiveByte()
{
    reinitUARTWithBaudrate(UART1W_tx_rx_rate);

    uint8_t byte = 0;
    txBuffer = UART1W_rx_byte;

    // Bits are received in UART-like order (LSB first)
    for(unsigned i = 0; i < 8; i++) {
        // The UART rate has been set to 115200 baud. Duration of a single bit
        // is now approximately 8.68μs (1/115200s). Receiving a single bit is
        // performed by pulling the data bus low for at least 1μs and then
        // waiting for reaction. The slave device keeps the bus low for at least
        // 15μs after the master pulse has begun to send '0', or leaves the bus
        // to be pulled high by the pull-up resistor to send '1'. Sending the
        // 0xff byte causes the bus to be pulled low for the duration of 1 bit
        // (start symbol). If the received byte is the same as the sent one, '1'
        // bit has been received. Otherwise, the received bit is '0'. Being
        // unable to receive any byte for more than UART1W_max_rx_delay_ms is
        // treated as if '1' has been received.

        byte >>= 1;

        rxBuffer = UART1W_rx_byte;
        uart->Receive(&rxBuffer, 1);
        uart->Send(&txBuffer, 1);

        if(xSemaphoreTake(uartRxComplete, UART1W_max_rx_delay_ms) == pdFALSE)
            uart->AbortRx();

        if(rxBuffer == txBuffer)
            byte |= 0x80;
    }
    return byte;
}

bool UART1W::SendByte(uint8_t byte)
{
    reinitUARTWithBaudrate(UART1W_tx_rx_rate);

    // Bits are sent in UART-like order (LSB first)
    for(unsigned i = 0; i < 8; i++) {
        // The UART rate has been set to 115200 baud. Duration of a single bit
        // is now approximately 8.68μs (1/115200s). Sending a single bit is
        // performed by pulling the bus low for 1-15μs (to send '1') or for
        // 60-120μs (to send '0'). Sending the 0xff byte causes the bus to be
        // pulled low for the duration of 1 bit (start symbol). On the other
        // hand, sending the 0x00 byte pulls the bus low for the duration of 9
        // bits (start symbol + 8 '0' bits) which is approximately 78.13μs. It
        // is not necessary to receive anything, but it can be done to make sure
        // that the bit has been sent successfully (and that there was no
        // collision). Receiving a byte differing from the sent one or being
        // unable to receive any byte for more than UART1W_max_rx_delay_ms is
        // treated as a fatal error.

        rxBuffer = UART1W_tx_dummy;
        uart->Receive(&rxBuffer, 1);

        txBuffer = (byte & 1 ? UART1W_tx_1_byte : UART1W_tx_0_byte);
        uart->Send(&txBuffer, 1);

        if(xSemaphoreTake(uartRxComplete, UART1W_max_rx_delay_ms) == pdFALSE)
            uart->AbortRx();

        if(rxBuffer != txBuffer)
            return false;

        byte >>= 1;
    }

    return true;
}

void UART1W::reinitUARTWithBaudrate(uint32_t baudrate)
{
    uart->SetBaudRate(baudrate);
    uart->ReInitialize();
}

void UART1W::uartRxCallback()
{
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(uartRxComplete, &pxHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(pxHigherPriorityTaskWoken)
}
