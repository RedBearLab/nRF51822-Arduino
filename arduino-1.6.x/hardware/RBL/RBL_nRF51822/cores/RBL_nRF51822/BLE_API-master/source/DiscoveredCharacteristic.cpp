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

#include "ble/DiscoveredCharacteristic.h"
#include "ble/GattClient.h"

ble_error_t
DiscoveredCharacteristic::read(uint16_t offset) const
{
    if (!props.read()) {
        return BLE_ERROR_OPERATION_NOT_PERMITTED;
    }

    if (!gattc) {
        return BLE_ERROR_INVALID_STATE;
    }

    return gattc->read(connHandle, valueHandle, offset);
}

ble_error_t
DiscoveredCharacteristic::write(uint16_t length, const uint8_t *value) const
{
    if (!props.write()) {
        return BLE_ERROR_OPERATION_NOT_PERMITTED;
    }

    if (!gattc) {
        return BLE_ERROR_INVALID_STATE;
    }

    return gattc->write(GattClient::GATT_OP_WRITE_REQ, connHandle, valueHandle, length, value);
}

ble_error_t
DiscoveredCharacteristic::writeWoResponse(uint16_t length, const uint8_t *value) const
{
    if (!props.writeWoResp()) {
        return BLE_ERROR_OPERATION_NOT_PERMITTED;
    }

    if (!gattc) {
        return BLE_ERROR_INVALID_STATE;
    }

    return gattc->write(GattClient::GATT_OP_WRITE_CMD, connHandle, valueHandle, length, value);
}

ble_error_t
DiscoveredCharacteristic::discoverDescriptors(DescriptorCallback_t callback, const UUID &matchingUUID) const
{
    return BLE_ERROR_NOT_IMPLEMENTED; /* TODO: this needs to be filled in. */
}
