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

#include "nRF51GattServer.h"
#include "mbed.h"

#include "common/common.h"
#include "btle/custom/custom_helper.h"

#include "nRF51Gap.h"

nRF51GattServer &nRF51GattServer::getInstance(void) {
    static nRF51GattServer m_instance;
    return m_instance;
}

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
    ASSERT( ERROR_NONE ==
            sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                     &nordicUUID,
                                     &serviceHandle),
            BLE_ERROR_PARAM_OUT_OF_RANGE );
    service.setHandle(serviceHandle);

    /* Add characteristics to the service */
    for (uint8_t i = 0; i < service.getCharacteristicCount(); i++) {
        GattCharacteristic *p_char = service.getCharacteristic(i);

        /* Skip any incompletely defined, read-only characteristics. */
        if ((p_char->getValueAttribute().getValuePtr() == NULL) &&
            (p_char->getValueAttribute().getInitialLength() == 0) &&
            (p_char->getProperties() == GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ)) {
            continue;
        }

        nordicUUID = custom_convert_to_nordic_uuid(p_char->getValueAttribute().getUUID());

        /* The user-description descriptor is a special case which needs to be
         * handled at the time of adding the characteristic. The following block
         * is meant to discover its presence. */
        const uint8_t *userDescriptionDescriptorValuePtr = NULL;
        uint16_t userDescriptionDescriptorValueLen = 0;
        for (uint8_t j = 0; j < p_char->getDescriptorCount(); j++) {
            GattAttribute *p_desc = p_char->getDescriptor(j);
            if (p_desc->getUUID() == BLE_UUID_DESCRIPTOR_CHAR_USER_DESC) {
                userDescriptionDescriptorValuePtr = p_desc->getValuePtr();
                userDescriptionDescriptorValueLen = p_desc->getLength();
            }
        }

        ASSERT ( ERROR_NONE ==
                 custom_add_in_characteristic(BLE_GATT_HANDLE_INVALID,
                                              &nordicUUID,
                                              p_char->getProperties(),
                                              p_char->getRequiredSecurity(),
                                              p_char->getValueAttribute().getValuePtr(),
                                              p_char->getValueAttribute().getInitialLength(),
                                              p_char->getValueAttribute().getMaxLength(),
                                              userDescriptionDescriptorValuePtr,
                                              userDescriptionDescriptorValueLen,
                                              p_char->isReadAuthorizationEnabled(),
                                              p_char->isWriteAuthorizationEnabled(),
                                              &nrfCharacteristicHandles[characteristicCount]),
                 BLE_ERROR_PARAM_OUT_OF_RANGE );

        /* Update the characteristic handle */
        p_characteristics[characteristicCount] = p_char;
        p_char->getValueAttribute().setHandle(nrfCharacteristicHandles[characteristicCount].value_handle);
        characteristicCount++;

        /* Add optional descriptors if any */
        /* ToDo: Make sure we don't overflow the array */
        for (uint8_t j = 0; j < p_char->getDescriptorCount(); j++) {
            GattAttribute *p_desc = p_char->getDescriptor(j);
            /* skip the user-description-descriptor here; this has already been handled when adding the characteristic (above). */
            if (p_desc->getUUID() == BLE_UUID_DESCRIPTOR_CHAR_USER_DESC) {
                continue;
            }

            nordicUUID = custom_convert_to_nordic_uuid(p_desc->getUUID());

            ASSERT(ERROR_NONE ==
                   custom_add_in_descriptor(BLE_GATT_HANDLE_INVALID,
                                            &nordicUUID,
                                            p_desc->getValuePtr(),
                                            p_desc->getInitialLength(),
                                            p_desc->getMaxLength(),
                                            &nrfDescriptorHandles[descriptorCount]),
                BLE_ERROR_PARAM_OUT_OF_RANGE);

            p_descriptors[descriptorCount++] = p_desc;
            p_desc->setHandle(nrfDescriptorHandles[descriptorCount]);
        }
    }

    serviceCount++;

    return BLE_ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief  Reads the value of a characteristic, based on the service
            and characteristic index fields

    @param[in]  attributeHandle
                The handle of the GattCharacteristic to read from
    @param[in]  buffer
                Buffer to hold the the characteristic's value
                (raw byte array in LSB format)
    @param[in/out] len
                input:  Length in bytes to be read.
                output: Total length of attribute value upon successful return.

    @returns    ble_error_t

    @retval     BLE_ERROR_NONE
                Everything executed properly
*/
/**************************************************************************/
ble_error_t nRF51GattServer::read(GattAttribute::Handle_t attributeHandle, uint8_t buffer[], uint16_t *lengthP)
{
    return read(BLE_CONN_HANDLE_INVALID, attributeHandle, buffer, lengthP);
}

ble_error_t nRF51GattServer::read(Gap::Handle_t connectionHandle, GattAttribute::Handle_t attributeHandle, uint8_t buffer[], uint16_t *lengthP)
{
    ble_gatts_value_t value = {
        .len     = *lengthP,
        .offset  = 0,
        .p_value = buffer,
    };

    ASSERT( ERROR_NONE ==
            sd_ble_gatts_value_get(connectionHandle, attributeHandle, &value),
            BLE_ERROR_PARAM_OUT_OF_RANGE);
    *lengthP = value.len;

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
*/
/**************************************************************************/
ble_error_t nRF51GattServer::write(GattAttribute::Handle_t attributeHandle, const uint8_t buffer[], uint16_t len, bool localOnly)
{
    return write(BLE_CONN_HANDLE_INVALID, attributeHandle, buffer, len, localOnly);
}

ble_error_t nRF51GattServer::write(Gap::Handle_t connectionHandle, GattAttribute::Handle_t attributeHandle, const uint8_t buffer[], uint16_t len, bool localOnly)
{
    uint16_t gapConnectionHandle = nRF51Gap::getInstance().getConnectionHandle();
    ble_error_t returnValue = BLE_ERROR_NONE;

    ble_gatts_value_t value = {
        .len     = len,
        .offset  = 0,
        .p_value = const_cast<uint8_t *>(buffer),
    };

    if (localOnly) {
        /* Only update locally regardless of notify/indicate */
        ASSERT_INT( ERROR_NONE,
                    sd_ble_gatts_value_set(connectionHandle, attributeHandle, &value),
                    BLE_ERROR_PARAM_OUT_OF_RANGE );
        return BLE_ERROR_NONE;
    }

    int characteristicIndex = resolveValueHandleToCharIndex(attributeHandle);
    if ((characteristicIndex != -1) &&
        (p_characteristics[characteristicIndex]->getProperties() & (GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY)) &&
        (gapConnectionHandle != connectionHandle)) {
        /* HVX update for the characteristic value */
        ble_gatts_hvx_params_t hvx_params;

        hvx_params.handle = attributeHandle;
        hvx_params.type   =
            (p_characteristics[characteristicIndex]->getProperties() & GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) ? BLE_GATT_HVX_NOTIFICATION : BLE_GATT_HVX_INDICATION;
        hvx_params.offset = 0;
        hvx_params.p_data = const_cast<uint8_t *>(buffer);
        hvx_params.p_len  = &len;

        error_t error = (error_t) sd_ble_gatts_hvx(gapConnectionHandle, &hvx_params);

        /* ERROR_INVALID_STATE, ERROR_BUSY, ERROR_GATTS_SYS_ATTR_MISSING and ERROR_NO_TX_BUFFERS the ATT table has been updated. */
        if ((error != ERROR_NONE) && (error != ERROR_INVALID_STATE) && (error != ERROR_BLE_NO_TX_BUFFERS) && (error != ERROR_BUSY) && (error != ERROR_BLEGATTS_SYS_ATTR_MISSING)) {
            ASSERT_INT( ERROR_NONE,
                        sd_ble_gatts_value_set(connectionHandle, attributeHandle, &value),
                        BLE_ERROR_PARAM_OUT_OF_RANGE );
        }

        /*  Notifications consume application buffers. The return value can
            be used for resending notifications.
        */
        if (error != ERROR_NONE) {
            returnValue = BLE_STACK_BUSY;
        }
    } else {
        ASSERT_INT( ERROR_NONE,
                    sd_ble_gatts_value_set(connectionHandle, attributeHandle, &value),
                    BLE_ERROR_PARAM_OUT_OF_RANGE );
    }

    return returnValue;
}

ble_error_t nRF51GattServer::areUpdatesEnabled(const GattCharacteristic &characteristic, bool *enabledP)
{
    /* Forward the call with the default connection handle. */
    return areUpdatesEnabled(nRF51Gap::getInstance().getConnectionHandle(), characteristic, enabledP);
}

ble_error_t nRF51GattServer::areUpdatesEnabled(Gap::Handle_t connectionHandle, const GattCharacteristic &characteristic, bool *enabledP)
{
    int characteristicIndex = resolveValueHandleToCharIndex(characteristic.getValueHandle());
    if (characteristicIndex == -1) {
        return BLE_ERROR_INVALID_PARAM;
    }

    /* Read the cccd value from the GATT server. */
    GattAttribute::Handle_t cccdHandle = nrfCharacteristicHandles[characteristicIndex].cccd_handle;
    uint16_t cccdValue;
    uint16_t length = sizeof(cccdValue);
    ble_error_t rc = read(connectionHandle, cccdHandle, reinterpret_cast<uint8_t *>(&cccdValue), &length);
    if (rc != BLE_ERROR_NONE) {
        return rc;
    }
    if (length != sizeof(cccdValue)) {
        return BLE_ERROR_INVALID_STATE;
    }

    /* Check for NOTFICATION or INDICATION in CCCD. */
    if ((cccdValue & BLE_GATT_HVX_NOTIFICATION) || (cccdValue & BLE_GATT_HVX_INDICATION)) {
        *enabledP = true;
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
    GattAttribute::Handle_t        handle_value;
    GattServerEvents::gattEvent_t  eventType;
    const ble_gatts_evt_t         *gattsEventP = &p_ble_evt->evt.gatts_evt;

    switch (p_ble_evt->header.evt_id) {
        case BLE_GATTS_EVT_WRITE: {
                /* There are 2 use case here: Values being updated & CCCD (indicate/notify) enabled */

                /* 1.) Handle CCCD changes */
                handle_value = gattsEventP->params.write.handle;
                int characteristicIndex = resolveCCCDHandleToCharIndex(handle_value);
                if ((characteristicIndex != -1) &&
                    (p_characteristics[characteristicIndex]->getProperties() &
                        (GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY))) {

                    uint16_t cccd_value = (gattsEventP->params.write.data[1] << 8) | gattsEventP->params.write.data[0]; /* Little Endian but M0 may be mis-aligned */

                    if (((p_characteristics[characteristicIndex]->getProperties() & GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE) && (cccd_value & BLE_GATT_HVX_INDICATION)) ||
                        ((p_characteristics[characteristicIndex]->getProperties() & GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) && (cccd_value & BLE_GATT_HVX_NOTIFICATION))) {
                        eventType = GattServerEvents::GATT_EVENT_UPDATES_ENABLED;
                    } else {
                        eventType = GattServerEvents::GATT_EVENT_UPDATES_DISABLED;
                    }

                    handleEvent(eventType, p_characteristics[characteristicIndex]->getValueHandle());
                    return;
                }

                /* 2.) Changes to the characteristic value will be handled with other events below */
                eventType = GattServerEvents::GATT_EVENT_DATA_WRITTEN;
            }
            break;

        case BLE_GATTS_EVT_HVC:
            /* Indication confirmation received */
            eventType    = GattServerEvents::GATT_EVENT_CONFIRMATION_RECEIVED;
            handle_value = gattsEventP->params.hvc.handle;
            break;

        case BLE_EVT_TX_COMPLETE: {
            handleDataSentEvent(p_ble_evt->evt.common_evt.params.tx_complete.count);
            return;
        }

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            sd_ble_gatts_sys_attr_set(gattsEventP->conn_handle, NULL, 0, 0);
            return;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            switch (gattsEventP->params.authorize_request.type) {
                case BLE_GATTS_AUTHORIZE_TYPE_READ:
                    eventType    = GattServerEvents::GATT_EVENT_READ_AUTHORIZATION_REQ;
                    handle_value = gattsEventP->params.authorize_request.request.read.handle;
                    break;
                case BLE_GATTS_AUTHORIZE_TYPE_WRITE:
                    eventType    = GattServerEvents::GATT_EVENT_WRITE_AUTHORIZATION_REQ;
                    handle_value = gattsEventP->params.authorize_request.request.write.handle;
                    break;
                default:
                    return;
            }
            break;

        default:
            return;
    }

    int characteristicIndex = resolveValueHandleToCharIndex(handle_value);
    if (characteristicIndex == -1) {
        return;
    }

    /* Find index (charHandle) in the pool */
    switch (eventType) {
        case GattServerEvents::GATT_EVENT_DATA_WRITTEN: {
            GattWriteCallbackParams cbParams = {
                .handle  = handle_value,
                .writeOp = static_cast<GattWriteCallbackParams::WriteOp_t>(gattsEventP->params.write.op),
                .offset  = gattsEventP->params.write.offset,
                .len     = gattsEventP->params.write.len,
                .data    = gattsEventP->params.write.data
            };
            handleDataWrittenEvent(&cbParams);
            break;
        }
        case GattServerEvents::GATT_EVENT_WRITE_AUTHORIZATION_REQ: {
            GattWriteAuthCallbackParams cbParams = {
                .handle  = handle_value,
                .offset  = gattsEventP->params.authorize_request.request.write.offset,
                .len     = gattsEventP->params.authorize_request.request.write.len,
                .data    = gattsEventP->params.authorize_request.request.write.data,
            };
            ble_gatts_rw_authorize_reply_params_t reply = {
                .type = BLE_GATTS_AUTHORIZE_TYPE_WRITE,
                .params = {
                    .write = {
                        .gatt_status = p_characteristics[characteristicIndex]->authorizeWrite(&cbParams)
                    }
                }
            };
            sd_ble_gatts_rw_authorize_reply(gattsEventP->conn_handle, &reply);

            /*
             * If write-authorization is enabled for a characteristic,
             * AUTHORIZATION_REQ event (if replied with true) is *not*
             * followed by another DATA_WRITTEN event; so we still need
             * to invoke handleDataWritten(), much the same as we would
             * have done if write-authorization had not been enabled.
             */
            if (reply.params.write.gatt_status == BLE_GATT_STATUS_SUCCESS) {
                GattWriteCallbackParams cbParams = {
                    .handle  = handle_value,
                    .writeOp = static_cast<GattWriteCallbackParams::WriteOp_t>(gattsEventP->params.authorize_request.request.write.op),
                    .offset  = gattsEventP->params.authorize_request.request.write.offset,
                    .len     = gattsEventP->params.authorize_request.request.write.len,
                    .data    = gattsEventP->params.authorize_request.request.write.data,
                };
                handleDataWrittenEvent(&cbParams);
            }
            break;
        }
        case GattServerEvents::GATT_EVENT_READ_AUTHORIZATION_REQ: {
            GattReadAuthCallbackParams cbParams = {
                .handle = handle_value,
                .offset = gattsEventP->params.authorize_request.request.read.offset,
                .len    = 0,
                .data   = NULL
            };

            ble_gatts_rw_authorize_reply_params_t reply = {
                .type = BLE_GATTS_AUTHORIZE_TYPE_READ,
                .params = {
                    .read = {
                        .gatt_status = p_characteristics[characteristicIndex]->authorizeRead(&cbParams)
                    }
                }
            };

            if (cbParams.authorizationReply == BLE_GATT_STATUS_SUCCESS) {
                if (cbParams.data != NULL) {
                    reply.params.read.update = 1;
                    reply.params.read.offset = cbParams.offset;
                    reply.params.read.len    = cbParams.len;
                    reply.params.read.p_data = cbParams.data;
                }
            }

            sd_ble_gatts_rw_authorize_reply(gattsEventP->conn_handle, &reply);
            break;
        }

        default:
            handleEvent(eventType, handle_value);
            break;
    }
}
