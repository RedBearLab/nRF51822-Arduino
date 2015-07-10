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

#ifndef __BLE_BATTERY_SERVICE_H__
#define __BLE_BATTERY_SERVICE_H__

#include "ble/BLE.h"

/**
* @class BatteryService
* @brief BLE Battery Service. This service displays the battery level from 0%->100% represented as a 8bit number.<br>
* Service:  https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.battery_service.xml <br>
* Battery Level Char:  https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.battery_level.xml
*/
class BatteryService {
public:
    /**
    * @param[ref] _ble
    *               BLE object for the underlying controller.
    * @param[in] level
    *               8bit batterly level. Usually used to represent percentage of batterly charge remaining.
    */
    BatteryService(BLE &_ble, uint8_t level = 100) :
        ble(_ble),
        batteryLevel(level),
        batteryLevelCharacteristic(GattCharacteristic::UUID_BATTERY_LEVEL_CHAR, &batteryLevel, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) {

        GattCharacteristic *charTable[] = {&batteryLevelCharacteristic};
        GattService         batteryService(GattService::UUID_BATTERY_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.addService(batteryService);
    }

    /**
     * @brief Update the battery level with a new value. Valid values range from
     * 0..100. Anything outside this range will be ignored.
     *
     * @param newLevel
     *              update to battery level.
     */
    void updateBatteryLevel(uint8_t newLevel) {
        batteryLevel = newLevel;
        ble.gattServer().write(batteryLevelCharacteristic.getValueHandle(), &batteryLevel, 1);
    }

protected:
    BLE &ble;

    uint8_t    batteryLevel;
    ReadOnlyGattCharacteristic<uint8_t> batteryLevelCharacteristic;
};

#endif /* #ifndef __BLE_BATTERY_SERVICE_H__*/
