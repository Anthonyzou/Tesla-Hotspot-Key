#include <HijelHID_BLEKeyboard.h>
#include <NimBLEDevice.h>

const int BUTTON_PIN = 0;
#define USER_LED 15

HijelHID_BLEKeyboard keyboard("AOTesla", "Hijel", 100);

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
    digitalWrite(USER_LED, LOW);

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