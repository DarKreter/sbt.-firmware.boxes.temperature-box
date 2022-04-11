add_executable(CanSender
        # Common files
        ../Common/common.cpp
        ../Common/vector_table.cpp
        # Starting procedure
        main.cpp
        # Application-specific tasks
        Tasks/Sender.cpp
        )

add_dependencies(CanSender STM32Cube-F1)
add_dependencies(CanSender FreeRTOS-Kernel)
add_dependencies(CanSender SBT-SDK)
