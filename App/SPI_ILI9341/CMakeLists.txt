add_executable(SPI_ILI9341
        # Common files
        ../Common/common.cpp
        ../Common/vector_table.cpp
        # Starting procedure
        main.cpp
        # Common tasks
        ../Common/Tasks/UARTGatekeeper.cpp
        # Application-specific tasks
        Tasks/SPI_ILI9341.cpp
        )

add_dependencies(SPI_ILI9341 STM32Cube-F1)
add_dependencies(SPI_ILI9341 FreeRTOS-Kernel)
add_dependencies(SPI_ILI9341 SBT-SDK)
