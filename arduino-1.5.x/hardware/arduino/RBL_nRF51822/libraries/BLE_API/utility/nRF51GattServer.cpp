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
 
#include "blecommon.h"
#include "nRF51GattServer.h"
#include "arduino.h"
#include "common.h"
#include "ble_error.h"

#include "custom_helper.h"

#include "nRF51Gap.h"
#include "nrf_err_check.h"

/**************************************************************************/
/*!
    @brief  Adds a new service to the GATT table on the peripheral

    @returns    ble_error_t

    @retval     BLE_ERROR_NONE
                Everything executed properly

    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
ble_error_t nRF51GattServer::addService(GattService &service)
{
    /* ToDo: Make sure we don't overflow the array, etc. */
    /* ToDo: Make sure this service UUID doesn't already exist (?) */
    /* ToDo: Basic validation */

    /* Add the service to the nRF51 */
    ble_uuid_t nordicUUID;
    nordicUUID = custom_convert_to_nordic_uuid(service.getUUID());

    uint16_t serviceHandle;
    // ASSERT( ERROR_NONE ==      //_modify
            // sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                     // &nordicUUID,
                                     // &serviceHandle),
            // BLE_ERROR_PARAM_OUT_OF_RANGE );
    nrf_err_check(sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                     &nordicUUID,
                                     &serviceHandle) );        
            
    service.setHandle(serviceHandle);

    /* Add characteristics to the service */
    for (uint8_t i = 0; i < service.getCharacteristicCount(); i++) {
        GattCharacteristic *p_char = service.getCharacteristic(i);

        nordicUUID = custom_convert_to_nordic_uuid(p_char->getUUID());

        // ASSERT ( ERROR_NONE ==    //_modify
                 // custom_add_in_characteristic(service.getHandle(),
                                              // &nordicUUID,
                                              // p_char->getProperties(),
                                              // NULL,
                                              // p_char->getInitialLength(),
                                              // p_char->getMaxLength(),
                                              // &nrfCharacteristicHandles[characteristicCount]),
                 // BLE_ERROR_PARAM_OUT_OF_RANGE );
        nrf_err_check(custom_add_in_characteristic(service.getHandle(),
                                              &nordicUUID,
                                              p_char->getProperties(),
                                              NULL,
                                              p_char->getInitialLength(),
                                              p_char->getMaxLength(),
                                              &nrfCharacteristicHandles[characteristicCount]));
                 
        /* Update the characteristic handle */
        uint16_t charHandle = characteristicCount;
        p_characteristics[characteristicCount++] = p_char;

        p_char->setHandle(charHandle);
        if ((p_char->getValuePtr() != NULL) && (p_char->getInitialLength() > 0)) {
            updateValue(charHandle, p_char->getValuePtr(), p_char->getInitialLength(), false /* localOnly */);
        }
    }

    serviceCount++;

    return BLE_ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief  Reads the value of a characteristic, based on the service
            and characteristic index fields

    @param[in]  charHandle
                The handle of the GattCharacteristic to read from
    @param[in]  buffer
                Buffer to hold the the characteristic's value
                (raw byte array in LSB format)
    @param[in]  len
                The number of bytes read into the buffer

    @returns    ble_error_t

    @retval     BLE_ERROR_NONE
                Everything executed properly

    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
ble_error_t nRF51GattServer::readValue(uint16_t charHandle, uint8_t buffer[], uint16_t *const lengthP)
{
    // ASSERT( ERROR_NONE ==  //_modify
            // sd_ble_gatts_value_get(nrfCharacteristicHandles[charHandle].value_handle, 0, lengthP, buffer),
            // BLE_ERROR_PARAM_OUT_OF_RANGE);

    nrf_err_check(sd_ble_gatts_value_get(nrfCharacteristicHandles[charHandle].value_handle, 0, lengthP, buffer));
            
    return BLE_ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief  Updates the value of a characteristic, based on the service
            and characteristic index fields

    @param[in]  charHandle
                The handle of the GattCharacteristic to write to
    @param[in]  buffer
                Data to use when updating the characteristic's value
                (raw byte array in LSB format)
    @param[in]  len
                The number of bytes in buffer

    @returns    ble_error_t

    @retval     BLE_ERROR_NONE
                Everything executed properly

    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
ble_error_t nRF51GattServer::updateValue(uint16_t charHandle, uint8_t buffer[], uint16_t len, bool localOnly)
{
    uint16_t gapConnectionHandle = nRF51Gap::getInstance().getConnectionHandle();

    if (localOnly) {
        /* Only update locally regardless of notify/indicate */
        // ASSERT_INT( ERROR_NONE,   //_modify
                    // sd_ble_gatts_value_set(nrfCharacteristicHandles[charHandle].value_handle, 0, &len, buffer),
                    // BLE_ERROR_PARAM_OUT_OF_RANGE );
                    
        nrf_err_check( sd_ble_gatts_value_set(nrfCharacteristicHandles[charHandle].value_handle, 0, &len, buffer) );            
    }

    if ((p_characteristics[charHandle]->getProperties() &
         (GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE |
          GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY)) &&
        (gapConnectionHandle != BLE_CONN_HANDLE_INVALID)) {
        /* HVX update for the characteristic value */
        ble_gatts_hvx_params_t hvx_params;

        hvx_params.handle = nrfCharacteristicHandles[charHandle].value_handle;
        hvx_params.type   =
            (p_characteristics[charHandle]->getProperties() &
             GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY)  ?
            BLE_GATT_HVX_NOTIFICATION : BLE_GATT_HVX_INDICATION;
        hvx_params.offset = 0;
        hvx_params.p_data = buffer;
        hvx_params.p_len  = &len;

        error_t error = (error_t) sd_ble_gatts_hvx(gapConnectionHandle, &hvx_params);

        /* ERROR_INVALID_STATE, ERROR_BUSY, ERROR_GATTS_SYS_ATTR_MISSING and
         *ERROR_NO_TX_BUFFERS the ATT table has been updated. */
        if ((error != ERROR_NONE) && (error != ERROR_INVALID_STATE) &&
                (error != ERROR_BLE_NO_TX_BUFFERS) && (error != ERROR_BUSY) &&
                (error != ERROR_BLEGATTS_SYS_ATTR_MISSING)) {
            // ASSERT_INT( ERROR_NONE,    //_modify
                        // sd_ble_gatts_value_set(nrfCharacteristicHandles[charHandle].value_handle, 0, &len, buffer),
                        // BLE_ERROR_PARAM_OUT_OF_RANGE );
            nrf_err_check( sd_ble_gatts_value_set(nrfCharacteristicHandles[charHandle].value_handle, 0, &len, buffer) );
        }
    } else {
        // ASSERT_INT( ERROR_NONE, //_modify
                    // sd_ble_gatts_value_set(nrfCharacteristicHandles[charHandle].value_handle, 0, &len, buffer),
                    // BLE_ERROR_PARAM_OUT_OF_RANGE );
        nrf_err_check( sd_ble_gatts_value_set(nrfCharacteristicHandles[charHandle].value_handle, 0, &len, buffer) );            
    }

    return BLE_ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief  Callback handler for events getting pushed up from the SD
*/
/**************************************************************************/
void nRF51GattServer::hwCallback(ble_evt_t *p_ble_evt)
{
    uint16_t                      handle_value;
    GattServerEvents::gattEvent_t event;

    switch (p_ble_evt->header.evt_id) {
        case BLE_GATTS_EVT_WRITE:
            /* There are 2 use case here: Values being updated & CCCD (indicate/notify) enabled */

            /* 1.) Handle CCCD changes */
            handle_value = p_ble_evt->evt.gatts_evt.params.write.handle;
            for (uint8_t i = 0; i<characteristicCount; i++) {
                if ((p_characteristics[i]->getProperties() & (GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY)) &&
                    (nrfCharacteristicHandles[i].cccd_handle == handle_value)) {
                    uint16_t cccd_value =
                        (p_ble_evt->evt.gatts_evt.params.write.data[1] << 8) |
                        p_ble_evt->evt.gatts_evt.params.write.data[0]; /* Little Endian but M0 may be mis-aligned */

                    if (((p_characteristics[i]->getProperties() & GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE) && (cccd_value & BLE_GATT_HVX_INDICATION)) ||
                        ((p_characteristics[i]->getProperties() & GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) && (cccd_value & BLE_GATT_HVX_NOTIFICATION))) {
                        event = GattServerEvents::GATT_EVENT_UPDATES_ENABLED;
                    } else {
                        event = GattServerEvents::GATT_EVENT_UPDATES_DISABLED;
                    }

                    handleEvent(event, i);
                    return;
                }
            }

            /* 2.) Changes to the characteristic value will be handled with other events below */
            event = GattServerEvents::GATT_EVENT_DATA_WRITTEN;
            break;

        case BLE_EVT_TX_COMPLETE:
            handleEvent(GattServerEvents::GATT_EVENT_DATA_SENT);
            return;

        case BLE_GATTS_EVT_HVC:
            /* Indication confirmation received */
            event        = GattServerEvents::GATT_EVENT_CONFIRMATION_RECEIVED;
            handle_value = p_ble_evt->evt.gatts_evt.params.hvc.handle;
            break;

        default:
            return;
    }

    /* Find index (charHandle) in the pool */
    for (uint8_t i = 0; i<characteristicCount; i++) {
        if (nrfCharacteristicHandles[i].value_handle == handle_value) {
            handleEvent(event, i);
            break;
        }
    }
}
