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

#ifndef __NRF51822_GAP_H__
#define __NRF51822_GAP_H__

#include "mbed.h"
#include "ble/blecommon.h"
#include "ble.h"
#include "ble/GapAdvertisingParams.h"
#include "ble/GapAdvertisingData.h"
#include "ble/Gap.h"
#include "ble/GapScanningParams.h"

#include "nrf_soc.h"
#include "ble_radio_notification.h"
#include "btle_security.h"

/**************************************************************************/
/*!
    \brief

*/
/**************************************************************************/
class nRF51Gap : public Gap
{
public:
    static nRF51Gap &getInstance();

    /* Functions that must be implemented from Gap */
    virtual ble_error_t setAddress(AddressType_t  type,  const Address_t address);
    virtual ble_error_t getAddress(AddressType_t *typeP, Address_t address);
    virtual ble_error_t setAdvertisingData(const GapAdvertisingData &, const GapAdvertisingData &);

    virtual uint16_t    getMinAdvertisingInterval(void) const {return ADVERTISEMENT_DURATION_UNITS_TO_MS(BLE_GAP_ADV_INTERVAL_MIN);}
    virtual uint16_t    getMinNonConnectableAdvertisingInterval(void) const {return ADVERTISEMENT_DURATION_UNITS_TO_MS(BLE_GAP_ADV_NONCON_INTERVAL_MIN);}
    virtual uint16_t    getMaxAdvertisingInterval(void) const {return ADVERTISEMENT_DURATION_UNITS_TO_MS(BLE_GAP_ADV_INTERVAL_MAX);}

    virtual ble_error_t startAdvertising(const GapAdvertisingParams &);
    virtual ble_error_t stopAdvertising(void);
    virtual ble_error_t connect(const Address_t, Gap::AddressType_t peerAddrType, const ConnectionParams_t *connectionParams, const GapScanningParams *scanParams);
    virtual ble_error_t disconnect(Handle_t connectionHandle, DisconnectionReason_t reason);
    virtual ble_error_t disconnect(DisconnectionReason_t reason);

    virtual ble_error_t setDeviceName(const uint8_t *deviceName);
    virtual ble_error_t getDeviceName(uint8_t *deviceName, unsigned *lengthP);
    virtual ble_error_t setAppearance(GapAdvertisingData::Appearance appearance);
    virtual ble_error_t getAppearance(GapAdvertisingData::Appearance *appearanceP);

    virtual ble_error_t setTxPower(int8_t txPower);
    virtual void        getPermittedTxPowerValues(const int8_t **valueArrayPP, size_t *countP);

    void     setConnectionHandle(uint16_t con_handle);
    uint16_t getConnectionHandle(void);

    virtual ble_error_t getPreferredConnectionParams(ConnectionParams_t *params);
    virtual ble_error_t setPreferredConnectionParams(const ConnectionParams_t *params);
    virtual ble_error_t updateConnectionParams(Handle_t handle, const ConnectionParams_t *params);

    virtual void onRadioNotification(RadioNotificationEventCallback_t callback) {
        Gap::onRadioNotification(callback);
        ble_radio_notification_init(NRF_APP_PRIORITY_HIGH, NRF_RADIO_NOTIFICATION_DISTANCE_800US, radioNotificationCallback);
    }

    virtual ble_error_t startRadioScan(const GapScanningParams &scanningParams) {
        ble_gap_scan_params_t scanParams = {
            .active      = scanningParams.getActiveScanning(), /**< If 1, perform active scanning (scan requests). */
            .selective   = 0,    /**< If 1, ignore unknown devices (non whitelisted). */
            .p_whitelist = NULL, /**< Pointer to whitelist, NULL if none is given. */
            .interval    = scanningParams.getInterval(),  /**< Scan interval between 0x0004 and 0x4000 in 0.625ms units (2.5ms to 10.24s). */
            .window      = scanningParams.getWindow(),    /**< Scan window between 0x0004 and 0x4000 in 0.625ms units (2.5ms to 10.24s). */
            .timeout     = scanningParams.getTimeout(),   /**< Scan timeout between 0x0001 and 0xFFFF in seconds, 0x0000 disables timeout. */
        };

        if (sd_ble_gap_scan_start(&scanParams) != NRF_SUCCESS) {
            return BLE_ERROR_PARAM_OUT_OF_RANGE;
        }

        return BLE_ERROR_NONE;
    }

    virtual ble_error_t stopScan(void) {
        if (sd_ble_gap_scan_stop() == NRF_SUCCESS) {
            return BLE_ERROR_NONE;
        }

        return BLE_STACK_BUSY;
    }

private:
    uint16_t m_connectionHandle;
    nRF51Gap() {
        m_connectionHandle = BLE_CONN_HANDLE_INVALID;
    }

    nRF51Gap(nRF51Gap const &);
    void operator=(nRF51Gap const &);
};

#endif // ifndef __NRF51822_GAP_H__
