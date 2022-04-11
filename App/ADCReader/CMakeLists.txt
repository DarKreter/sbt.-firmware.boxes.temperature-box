add_executable(ADCReader
        # Common files
        ../Common/common.cpp
        ../Common/vector_table.cpp
        # Starting procedure
        main.cpp
        # Common tasks
        ../Common/Tasks/UARTGatekeeper.cpp
        # Application-specific tasks
        Tasks/ADCReader.cpp
        )

add_dependencies(ADCReader STM32Cube-F1)
add_dependencies(ADCReader FreeRTOS-Kernel)
add_dependencies(ADCReader SBT-SDK)
