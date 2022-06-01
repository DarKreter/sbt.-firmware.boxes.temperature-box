//
// Created by hubert25632 on 09.05.22.
//

#ifndef TEMPERATURE_BOX_PROJECT_DS18B20_HPP
#define TEMPERATURE_BOX_PROJECT_DS18B20_HPP

#include "UART1W.hpp"

class DS18B20 {
    enum class Command : uint8_t {
        ConvertT = 0x44,
        CopyScratchpad = 0x48,
        WriteScratchpad = 0x4e,
        ReadScratchpad = 0xbe,
        SkipROM = 0xcc
    };

    class MaximCRC {
        uint8_t crc = 0;

    public:
        void Reset();
        void InputByte(uint8_t byte);
        [[nodiscard]] uint8_t GetCRC() const;
    };

    MaximCRC maximCrc;
    UART1W uart1W;

    inline bool sendCommand(Command command)
    {
        return uart1W.SendByte(static_cast<uint8_t>(command));
    }

public:
    explicit DS18B20(SBT::Hardware::UART* uart);

    /// Get temperature from the device
    /// \return Current temperature if the read operation succeeds, nothing
    /// otherwise
    std::optional<int16_t> GetTemperature();
};

#endif // TEMPERATURE_BOX_PROJECT_DS18B20_HPP
