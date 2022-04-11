add_executable(CanReceiver
        # Common files
        ../Common/common.cpp
        ../Common/vector_table.cpp
        # Starting procedure
        main.cpp
        # Application-specific tasks
        Tasks/Receiver.cpp
        )

add_dependencies(CanReceiver STM32Cube-F1)
add_dependencies(CanReceiver FreeRTOS-Kernel)
add_dependencies(CanReceiver SBT-SDK)
