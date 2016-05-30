/*
 * Copyright (c) 2016 RedBear
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include <BLE_API.h>

BLEDevice ble;

const static uint8_t beaconPayload[] = {
  0x4C,0x00,                                                                       // Company Identifier Code = Apple
  0x02,                                                                            // Type = iBeacon
  0x15,                                                                            // Following data length
  0x71,0x3d,0x00,0x00,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e, // Beacon UUID
  0x00,0x00,                                                                       // Major
  0x00,0x00,                                                                       // Minor
  0xC5                                                                             // Measure Power
};

void setup() {
  // put your setup code here, to run once
  // close peripheral power
  NRF_POWER->DCDCEN = 0x00000001;
  NRF_TIMER1->POWER = 0;
  NRF_TIMER2->POWER = 0;
  NRF_UART0->POWER  = 0;
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  
  ble.init(); 
  // set advertisement
  ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
  ble.accumulateAdvertisingPayload(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, beaconPayload, sizeof(beaconPayload));
  // set advertise type  
  //  ADV_CONNECTABLE_UNDIRECTED
  //  ADV_CONNECTABLE_DIRECTED
  //  ADV_SCANNABLE_UNDIRECTED
  //  ADV_NON_CONNECTABLE_UNDIRECTED
  ble.setAdvertisingType(GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED);
  // 100ms; in multiples of 0.625ms
  ble.setAdvertisingInterval(160); 
  // set adv_timeout, in seconds
  ble.setAdvertisingTimeout(0);
  // start advertising
  ble.startAdvertising();
}

void loop() {
  // put your main code here, to run repeatedly:
  ble.waitForEvent();
}

