include_directories(Common)

link_libraries(STM32Cube-F1)
link_libraries(FreeRTOS-Kernel)
link_libraries(SBT-SDK)

add_subdirectory(UARTEcho)
add_subdirectory(I2C_24AA01)
add_subdirectory(SPI_ILI9341)
add_subdirectory(ADCReader)
add_subdirectory(CanSender)
add_subdirectory(CanReceiver)
