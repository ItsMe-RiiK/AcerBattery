#!/bin/bash
# Move to the project directory no matter where this script is called from
cd "$(dirname "$0")"

if [ -f "RKKDR/Makefile" ]; then
    KERNEL_DIR="RKKDR"
elif [ -f "../KernelDriver/Makefile" ]; then
    KERNEL_DIR="../KernelDriver"
else
    echo "Error: RKKDR module source not found."
    exit 1
fi
RELEASE_DIR="release"

# 1. Compile the GUI natively
make

# 2. Check and load the Kernel Module
if ! lsmod | grep -q RKKDR; then
    # If not loaded, compile and load it automatically
    make -C "$KERNEL_DIR" load
fi

# 3. Allow root to access the X11 display
xhost +si:localuser:root > /dev/null 2>&1

# 4. Launch the GUI
BINARY="$PWD/$RELEASE_DIR/AcerBattery"
pkexec env DISPLAY="$DISPLAY" XAUTHORITY="$XAUTHORITY" "$BINARY" > gui_error.log 2>&1
