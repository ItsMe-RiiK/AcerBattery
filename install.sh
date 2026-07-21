#!/bin/bash
set -e

cd "$(dirname "$0")"

echo "=== AcerBattery Setup ==="

# 1. Handle Kernel Module Source
if [ -f "../KernelDriver/Makefile" ]; then
    echo "[*] Developer environment detected. Using ../KernelDriver."
    KERNEL_DIR="../KernelDriver"
else
    # Standard user
    if [ ! -f "RKKDR/Makefile" ]; then
        echo "[*] Initializing RKKDR submodule..."
        git submodule update --init --recursive
    else
        echo "[*] RKKDR submodule is already initialized."
    fi
    KERNEL_DIR="RKKDR"
fi

# 2. Build AcerBattery GUI
echo ""
echo "[*] Building AcerBattery GUI..."
make

# 3. Install RKKDR Kernel Driver via DKMS
echo ""
echo "[*] Installing RKKDR Kernel Module via DKMS..."
if [ -f "$KERNEL_DIR/install-dkms.sh" ]; then
    (cd "$KERNEL_DIR" && bash ./install-dkms.sh)
    
    echo "[*] Loading RKKDR module..."
    sudo modprobe RKKDR || true
else
    echo "[!] Error: install-dkms.sh not found in $KERNEL_DIR."
    echo "[!] Please ensure the submodule is correctly populated."
    exit 1
fi

echo ""
echo "=== Installation Complete ==="
echo "You can now run AcerBattery by executing ./launcher.sh"
