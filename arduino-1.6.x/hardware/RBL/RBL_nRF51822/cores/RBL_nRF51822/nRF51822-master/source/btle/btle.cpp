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

#include "common/common.h"
#include "nordic_common.h"

#include "btle.h"

#include "ble_stack_handler_types.h"
#include "ble_flash.h"
#include "ble_conn_params.h"

#include "btle_gap.h"
#include "btle_advertising.h"
#include "custom/custom_helper.h"

#include "softdevice_handler.h"
#include "pstorage.h"

#include "ble/GapEvents.h"
#include "nRF51Gap.h"
#include "nRF51GattServer.h"
#include "nRF51SecurityManager.h"

#include "device_manager.h"

#include "ble_hci.h"
#include "btle_discovery.h"

extern "C" void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name);
void            app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t *p_file_name);

static void btle_handler(ble_evt_t *p_ble_evt);

static void sys_evt_dispatch(uint32_t sys_evt)
{
    pstorage_sys_event_handler(sys_evt);
}

error_t btle_init(void)
{
#if defined(TARGET_DELTA_DFCM_NNN40) || defined(TARGET_HRM1017)
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_4000MS_CALIBRATION, NULL);
#else
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, NULL);
#endif

    // Enable BLE stack
    /**
     * Using this call, the application can select whether to include the
     * Service Changed characteristic in the GATT Server. The default in all
     * previous releases has been to include the Service Changed characteristic,
     * but this affects how GATT clients behave. Specifically, it requires
     * clients to subscribe to this attribute and not to cache attribute handles
     * between connections unless the devices are bonded. If the application
     * does not need to change the structure of the GATT server attributes at
     * runtime this adds unnecessary complexity to the interaction with peer
     * clients. If the SoftDevice is enabled with the Service Changed
     * Characteristics turned off, then clients are allowed to cache attribute
     * handles making applications simpler on both sides.
     */
    static const bool IS_SRVC_CHANGED_CHARACT_PRESENT = true;
    ble_enable_params_t enableParams = {
        .gatts_enable_params = {
            .service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT
        }
    };
    if (sd_ble_enable(&enableParams) != NRF_SUCCESS) {
        return ERROR_INVALID_PARAM;
    }

    ble_gap_addr_t addr;
    if (sd_ble_gap_address_get(&addr) != NRF_SUCCESS) {
        return ERROR_INVALID_PARAM;
    }
    if (sd_ble_gap_address_set(BLE_GAP_ADDR_CYCLE_MODE_NONE, &addr) != NRF_SUCCESS) {
        return ERROR_INVALID_PARAM;
    }

    ASSERT_STATUS( softdevice_ble_evt_handler_set(btle_handler));
    ASSERT_STATUS( softdevice_sys_evt_handler_set(sys_evt_dispatch));

    btle_gap_init();

    return ERROR_NONE;
}

static void btle_handler(ble_evt_t *p_ble_evt)
{
    /* Library service handlers */
#if SDK_CONN_PARAMS_MODULE_ENABLE
    ble_conn_params_on_ble_evt(p_ble_evt);
#endif

    dm_ble_evt_handler(p_ble_evt);

    bleGattcEventHandler(p_ble_evt);

    /* Custom event handler */
    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED: {
            Gap::Handle_t handle = p_ble_evt->evt.gap_evt.conn_handle;
            nRF51Gap::getInstance().setConnectionHandle(handle);
            const Gap::ConnectionParams_t *params = reinterpret_cast<Gap::ConnectionParams_t *>(&(p_ble_evt->evt.gap_evt.params.connected.conn_params));
            const ble_gap_addr_t *peer = &p_ble_evt->evt.gap_evt.params.connected.peer_addr;
            const ble_gap_addr_t *own  = &p_ble_evt->evt.gap_evt.params.connected.own_addr;
            nRF51Gap::getInstance().processConnectionEvent(handle,
                                                           static_cast<Gap::Role_t>(p_ble_evt->evt.gap_evt.params.connected.role),
                                                           static_cast<Gap::AddressType_t>(peer->addr_type), peer->addr,
                                                           static_cast<Gap::AddressType_t>(own->addr_type),  own->addr,
                                                           params);
            break;
        }

        case BLE_GAP_EVT_DISCONNECTED: {
            Gap::Handle_t handle = p_ble_evt->evt.gap_evt.conn_handle;
            // Since we are not in a connection and have not started advertising,
            // store bonds
            nRF51Gap::getInstance().setConnectionHandle (BLE_CONN_HANDLE_INVALID);

            Gap::DisconnectionReason_t reason;
            switch (p_ble_evt->evt.gap_evt.params.disconnected.reason) {
                case BLE_HCI_LOCAL_HOST_TERMINATED_CONNECTION:
                    reason = Gap::LOCAL_HOST_TERMINATED_CONNECTION;
                    break;
                case BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION:
                    reason = Gap::REMOTE_USER_TERMINATED_CONNECTION;
                    break;
                case BLE_HCI_CONN_INTERVAL_UNACCEPTABLE:
                    reason = Gap::CONN_INTERVAL_UNACCEPTABLE;
                    break;
                default:
                    /* Please refer to the underlying transport library for an
                     * interpretion of this reason's value. */
                    reason = static_cast<Gap::DisconnectionReason_t>(p_ble_evt->evt.gap_evt.params.disconnected.reason);
                    break;
            }
            nRF51Gap::getInstance().processDisconnectionEvent(handle, reason);
            break;
        }

        case BLE_GAP_EVT_PASSKEY_DISPLAY:
            nRF51SecurityManager::getInstance().processPasskeyDisplayEvent(p_ble_evt->evt.gap_evt.conn_handle, p_ble_evt->evt.gap_evt.params.passkey_display.passkey);
            break;

        case BLE_GAP_EVT_TIMEOUT:
            nRF51Gap::getInstance().processTimeoutEvent(static_cast<Gap::TimeoutSource_t>(p_ble_evt->evt.gap_evt.params.timeout.src));
            break;

        case BLE_GATTC_EVT_TIMEOUT:
        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server and Client timeout events.
            // ASSERT_STATUS_RET_VOID (sd_ble_gap_disconnect(m_conn_handle,
            // BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION));
            break;

        case BLE_GAP_EVT_ADV_REPORT: {
            const ble_gap_evt_adv_report_t *advReport = &p_ble_evt->evt.gap_evt.params.adv_report;
            nRF51Gap::getInstance().processAdvertisementReport(advReport->peer_addr.addr,
                                                               advReport->rssi,
                                                               advReport->scan_rsp,
                                                               static_cast<GapAdvertisingParams::AdvertisingType_t>(advReport->type),
                                                               advReport->dlen,
                                                               advReport->data);
            break;
        }

        default:
            break;
    }

    nRF51GattServer::getInstance().hwCallback(p_ble_evt);
}

/*! @brief      Callback when an error occurs inside the SoftDevice */
void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name)
{
    ASSERT(false, (void) 0);
}

/*!
    @brief      Handler for general errors above the SoftDevice layer.
                Typically we can' recover from this so we do a reset.
*/
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t *p_file_name)
{
    ASSERT_STATUS_RET_VOID( error_code );
    NVIC_SystemReset();
}
