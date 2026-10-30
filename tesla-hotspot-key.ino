#include <HijelHID_BLEKeyboard.h>
#include <NimBLEDevice.h>

const int BUTTON_PIN = 0;

// Override per board with -DUSER_LED=<gpio> / -DLED_ON=LOW (see build.sh).
// Some S3 dev board revisions wire the RGB LED to GPIO38 instead of the variant's 48.
#ifndef USER_LED
  #ifdef LED_BUILTIN
    #define USER_LED LED_BUILTIN
  #else
    #define USER_LED 15
  #endif
#endif
#ifndef LED_ON
  #define LED_ON HIGH
#endif

HijelHID_BLEKeyboard keyboard("test2", "Hijel", 100);

/**

Libraries used:

https://github.com/HijelHub/HijelHID_BLEKeyboard/blob/main/src/HijelHID_BLEKeyboard.cpp
https://github.com/h2zero/NimBLE-Arduino

*/

void setup() {
    // Serial.begin(115200);

    // USB CDC on boot OFF in production, ON in dev

    Serial.println("HijelHID BLE Keyboard — Basic Example");

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    keyboard.setLogLevel(HIDLogLevel::Normal);

    // set led on
    pinMode(USER_LED, OUTPUT);
    rgbLedWrite(USER_LED, 1, 1, 1);  // 0–255 per channel; core default via digitalWrite is 64

    // start
    keyboard.begin();

    // set tx power
    NimBLEDevice::setPower(-24);

    // --- STRICT BONDING FILTER ---
    // If your phone is already bonded, restrict connections ONLY to bonded devices
    if (NimBLEDevice::getNumBonds() > 0) {
        NimBLEDevice::whiteListAdd(NimBLEDevice::getBondedAddress(0));
        NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
        pAdvertising->setScanFilter(false, true); // (allowScanFromAll, allowConnectOnlyFromWhitelist)
        Serial.println("Device locked: Only accepting bonded phone!");
    } else {
        Serial.println("No bond found: Waiting for initial pairing...");
    }
}

void loop() {
    delay(10000);
}