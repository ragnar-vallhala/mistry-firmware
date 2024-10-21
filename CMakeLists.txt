cmake_minimum_required(VERSION 3.13)


# Setting Variables
set(project Mistry)
set(PICO_SDK_PATH "/home/ragnar/pico-sdk")
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(FREERTOS_SRC_DIRECTORY dependencies/FreeRTOS-Kernel)
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
)
# Create an executable from source files
add_executable(${project}
  ${SOURCES}  
)


include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/include 
    ${CMAKE_CURRENT_LIST_DIR}/
)

target_include_directories(Mistry PRIVATE ${FREERTOS_SRC_DIRECTORY}/include)
target_include_directories(Mistry PRIVATE ${FREERTOS_SRC_DIRECTORY}/portable/GCC/RP2040)

target_link_libraries(${project} pico_stdlib hardware_gpio hardware_i2c  FreeRTOS-Kernel-Heap4 pico_multicore )

# Enable USB and UART
pico_enable_stdio_usb(${project} 1)
pico_enable_stdio_uart(${project} 1)

# Create a UF2 file for flashing the Pico
pico_add_extra_outputs(${project})

