cmake_minimum_required(VERSION 3.13)


# Set the path to the Pico SDK
set(PICO_SDK_PATH "/home/ragnar/pico-sdk")
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include the Pico SDK
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(freertos_pico_project C CXX ASM)
# Initialize the Pico SDK
pico_sdk_init()

# Create an executable from your source files
add_executable(freertos_pico
    src/main.c
)
# Link FreeRTOS, the config, and the Pico SDK to your project
target_link_libraries(freertos_pico pico_stdlib hardware_gpio )

# Enable USB and UART
pico_enable_stdio_usb(freertos_pico 1)
pico_enable_stdio_uart(freertos_pico 0)

# Create a UF2 file for flashing the Pico
pico_add_extra_outputs(freertos_pico)

