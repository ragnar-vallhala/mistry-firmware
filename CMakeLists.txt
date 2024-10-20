cmake_minimum_required(VERSION 3.13)


# Setting Variables
set(project Mistry)
set(PICO_SDK_PATH "/home/ragnar/pico-sdk")
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Include the Pico SDK
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
# Set Project
project(${project} C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

# Include Directory
include_directories(
include
)

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


# Link libraries to the executable
target_link_libraries(${project} pico_stdlib hardware_gpio hardware_i2c)


# Enable USB and UART
pico_enable_stdio_usb(${project} 1)
pico_enable_stdio_uart(${project} 0)

# Create a UF2 file for flashing the Pico
pico_add_extra_outputs(${project})

