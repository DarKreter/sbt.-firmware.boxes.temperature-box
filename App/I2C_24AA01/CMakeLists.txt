add_executable(I2C_24AA01
        # Common files
        ../Common/common.cpp
        ../Common/vector_table.cpp
        # Starting procedure
        main.cpp
        # Common libraries
        ../Common/Libs/byte2hex.cpp
        # Common tasks
        ../Common/Tasks/UARTGatekeeper.cpp
        # Application-specific tasks
        Tasks/I2C_24AA01.cpp
        )

add_dependencies(I2C_24AA01 STM32Cube-F1)
add_dependencies(I2C_24AA01 FreeRTOS-Kernel)
add_dependencies(I2C_24AA01 SBT-SDK)
