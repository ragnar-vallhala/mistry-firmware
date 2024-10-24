#!/bin/bash

# Path to the compiled .uf2 file
UF2_PATH="./build/Mistry.uf2"

# Check if picotool is installed
if ! command -v picotool &> /dev/null
then
    echo "picotool could not be found. Please install it first."
    exit 1
fi

# Check if the UF2 file exists
if [ ! -f "$UF2_PATH" ]; then
    echo "UF2 file not found at $UF2_PATH"
    exit 1
fi

# Try to detect the Pico connected via USB (without needing BOOTSEL mode)
echo "Checking for connected Pico devices..."

if picotool info &> /dev/null; then
    echo "Pico detected, flashing the UF2 file..."

    # Flash the UF2 file to the Pico
    picotool load "$UF2_PATH" --verify

    if [ $? -eq 0 ]; then
        echo "Upload successful!"
    else
        echo "Failed to upload UF2."
        exit 1
    fi
else
    echo "No Pico device detected. Ensure the device is connected."
    exit 1
fi

