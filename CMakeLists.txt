cmake_minimum_required(VERSION 3.13)


# Setting Variables
set(project Mistry)
set(PICO_SDK_PATH "/home/ragnar/pico-sdk")
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(FREERTOS_SRC_DIRECTORY dependencies/FreeRTOS-Kernel)
set(PICO_BOARD pico_w)
# Include the Pico SDK
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

include(${FREERTOS_SRC_DIRECTORY}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)
# Set Project
project(${project} C CXX ASM)
# Initialize the Pico SDK
pico_sdk_init()

# All source files to build
file(GLOB SOURCES 
  src/*.cpp
  src/connection/*.cpp
  src/display/*.cpp
  src/resource/*.cpp
  src/keypad/*.cpp
  src/experiments/*.cpp
  src/experiments/kinematics/*.cpp
  src/experiments/dynamics/*.cpp
  src/experiments/gravity/*.cpp
  src/hx711/*.cpp
  src/microphone/*.cpp
  src/setting/*.cpp
)
# Create an executable from source files
add_executable(${project}
  ${SOURCES}  
)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/include 
    ${CMAKE_CURRENT_LIST_DIR}/
)



target_link_libraries(${project} pico_stdlib hardware_gpio hardware_adc hardware_i2c pico_cyw43_arch_lwip_threadsafe_background  FreeRTOS-Kernel-Heap4 pico_multicore m)

# Enable USB and UART
pico_enable_stdio_usb(${project} 1)
pico_enable_stdio_uart(${project} 1)

# Create a UF2 file for flashing the Pico
pico_add_extra_outputs(${project})
