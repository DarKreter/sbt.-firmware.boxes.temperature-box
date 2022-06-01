if (DEFINED ENV{SBT_TEMP_CANTEST})
    set(TEMP_CANTEST Tasks/CANPrint.cpp)
endif ()

add_executable(Firmware
        # Common files
        ../Common/common.cpp
        ../Common/vector_table.cpp
        # Starting procedure
        main.cpp
        # Application-specific tasks
        Lib/UART1W.cpp
        Lib/DS18B20.cpp
        Tasks/ReadTemp.cpp
        Tasks/TempCAN.cpp
        ${TEMP_CANTEST}
        )

include_directories(Tasks Lib)

add_dependencies(Firmware STM32Cube-F1)
add_dependencies(Firmware FreeRTOS-Kernel)
add_dependencies(Firmware SBT-SDK)
