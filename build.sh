#!/usr/bin/env bash
set -euo pipefail

PORT=/dev/cu.usbmodem101
FQBN=esp32:esp32:esp32s3
# CDCOnBoot=cdc routes Serial to the native USB port (dev); drop it for production.
OPTS= # CDCOnBoot=cdc
# Optional LED overrides, e.g. LED_FLAGS="-DUSER_LED=38" or "-DUSER_LED=15 -DLED_ON=LOW"
LED_FLAGS=${LED_FLAGS:-}

setup() {
  arduino-cli core install esp32:esp32
  arduino-cli lib install HijelHID_BLEKeyboard NimBLE-Arduino
}

build()   { arduino-cli compile --fqbn "$FQBN" --board-options "$OPTS" --build-property "compiler.cpp.extra_flags=$LED_FLAGS" . ; }
upload()  { arduino-cli upload -p "$PORT" --fqbn "$FQBN" --board-options "$OPTS" . ; }
monitor() { arduino-cli monitor -p "$PORT" --config 115200 ; }
# Regenerate VS Code IntelliSense config (re-run after changing board, options, or libraries).
ide()     {
  arduino-cli compile --fqbn "$FQBN" --board-options "$OPTS" --build-property "compiler.cpp.extra_flags=$LED_FLAGS" \
    --only-compilation-database --build-path ./build .
  python3 gen-intellisense.py
}
bandu()   { build && upload ; }
all()     { build && upload && monitor ; }

"${@:-all}"
