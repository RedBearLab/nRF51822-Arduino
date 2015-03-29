/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include <BLE_API.h>

BLEDevice ble;

const static uint8_t beaconPayload[] = {
    0x4C, 0x00,                                             // Company identifier code (0x004C == Apple)
    0x02,                                                   // ID
    0x15,                                                   // length of the remaining payload
    0xE2, 0xC5, 0x6D, 0xB5, 0xDF, 0xFB, 0x48, 0xD2,         // location UUID
    0xB0, 0x60, 0xD0, 0xF5, 0xA7, 0x10, 0x96, 0xE0,
    0x00, 0x00,                                             // the major value to differentiate a location
    0x00, 0x00,                                             // the minor value to differentiate a location
    0xC8                                                    // 2's complement of the Tx power (-56dB)
};

void setup() 
{
    uint32_t err_code = 0;
    uint8_t val = 0;
  
    Serial.begin(115200);
    Serial.println("BLE Init!");
    
    err_code = ble.init(); 
    
    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, beaconPayload, sizeof(beaconPayload));
    ble.setAdvertisingType(GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED);
    ble.setAdvertisingInterval(160); /* 100ms; in multiples of 0.625ms. */

    ble.startAdvertising();

    Serial.println("BLE Start Advertising!");
}

void loop() 
{
    ble.waitForEvent();
}
