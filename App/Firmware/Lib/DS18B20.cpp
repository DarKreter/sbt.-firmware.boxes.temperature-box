//
// Created by hubert25632 on 09.05.22.
//

#include "DS18B20.hpp"

// It takes at most 750ms to perform a single temperature conversion
constexpr TickType_t DS18B20_temp_conv_time_ms = 750;

void DS18B20::MaximCRC::Reset() { crc = 0; }

void DS18B20::MaximCRC::InputByte(uint8_t byte)
{
    for(unsigned i = 0; i < 8; i++) {
        // This code is an implementation of the CRC checksum generator with
        // equivalent polynomial function:
        // CRC = x^8 + x^5 + x^4 + 1
        // See 'Figure 11. CRC Generator' in DS18B20 datasheet.

        uint8_t xored_input = (byte & 1) ^ (crc & 1);
        crc >>= 1;
        crc |= (xored_input << 7);
        crc ^= (xored_input * 0x0c);
        byte >>= 1;
    }
}

uint8_t DS18B20::MaximCRC::GetCRC() const { return crc; }

DS18B20::DS18B20(SBT::Hardware::UART* uart) : uart1W(uart) {}

std::optional<int16_t> DS18B20::GetTemperature()
{
    if(!uart1W.Reset())
        return std::nullopt;
    if(!sendCommand(Command::SkipROM))
        return std::nullopt;
    if(!sendCommand(Command::ConvertT))
        return std::nullopt;

    vTaskDelay(DS18B20_temp_conv_time_ms);

    if(!uart1W.Reset())
        return std::nullopt;
    if(!sendCommand(Command::SkipROM))
        return std::nullopt;
    if(!sendCommand(Command::ReadScratchpad))
        return std::nullopt;

    uint8_t scratchpad[9];
    maximCrc.Reset();
    for(auto& i : scratchpad) {
        i = uart1W.ReceiveByte();
        maximCrc.InputByte(i);
    }

    if(maximCrc.GetCRC() != 0)
        return std::nullopt;

    union {
        int16_t temp;
        uint8_t temp_bytes[2];
    } temp_union;

    for(unsigned i = 0; i < 2; i++)
        temp_union.temp_bytes[i] = scratchpad[i];

    return temp_union.temp;
}
