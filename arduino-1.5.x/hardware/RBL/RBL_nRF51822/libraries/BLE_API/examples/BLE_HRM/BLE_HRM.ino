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

#define HRM_TIME                             APP_TIMER_TICKS(1000, 0)

BLEDevice  ble;

const static char  DEVICE_NAME[] = "Nordic_HRM";

static uint8_t hrmCounter     = 100;
static uint8_t bpm[2]         = {0x00, hrmCounter};
static const uint8_t location = 0x03;       
static uint32_t   cnt;

static app_timer_id_t                        m_hrs_timer_id;

GattCharacteristic hrmRate(GattCharacteristic::UUID_HEART_RATE_MEASUREMENT_CHAR, bpm, sizeof(bpm), sizeof(bpm), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

GattCharacteristic hrmLocation(GattCharacteristic::UUID_BODY_SENSOR_LOCATION_CHAR,(uint8_t *)&location, sizeof(location), sizeof(location),
                               GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ);
                               
GattCharacteristic *hrmChars[] = {&hrmRate, &hrmLocation, };

GattService        hrmService(GattService::UUID_HEART_RATE_SERVICE, hrmChars, sizeof(hrmChars) / sizeof(GattCharacteristic *));

static const uint16_t uuid16_list[] = {GattService::UUID_HEART_RATE_SERVICE};


void disconnectionCallback(void)
{
    Serial.println("Disconnected!");
    Serial.println("Restarting the advertising process");
    ble.startAdvertising();
}

void periodicCallback( void * p_context )
{
    if (ble.getGapState().connected) 
    {
        /* Update the HRM measurement */
        /* First byte = 8-bit values, no extra info, Second byte = uint8_t HRM value */
        /* See --> https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.heart_rate_measurement.xml */
        hrmCounter++;
        if (hrmCounter == 175) 
            hrmCounter = 100;
            
        bpm[1] = hrmCounter;
        ble.updateCharacteristicValue(hrmRate.getHandle(), bpm, sizeof(bpm));
    }
}
void setup(void)
{    
    uint32_t err_code;  
  
    Serial.begin(115200);
    Serial.println("Initialising the nRF51822");
    ble.init();
    
    err_code = app_timer_create(&m_hrs_timer_id, APP_TIMER_MODE_REPEATED, periodicCallback );
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_hrs_timer_id, HRM_TIME, NULL);
    APP_ERROR_CHECK(err_code);	
    
    ble.onDisconnection(disconnectionCallback);
    
    Serial.println("setup advertising");
    /* setup advertising */
    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t*)uuid16_list, sizeof(uuid16_list));
    ble.accumulateAdvertisingPayload(GapAdvertisingData::HEART_RATE_SENSOR_HEART_RATE_BELT);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.setAdvertisingInterval(160); /* 100ms; in multiples of 0.625ms. */
    
    ble.addService(hrmService);
    
    //Set Dev_Name
    err_code = RBL_SetDevName("BLE HRM");
    APP_ERROR_CHECK(err_code);    
    
    ble.startAdvertising();
    
    Serial.println("start advertising");
}

void loop(void)
{
    ble.waitForEvent();
}
