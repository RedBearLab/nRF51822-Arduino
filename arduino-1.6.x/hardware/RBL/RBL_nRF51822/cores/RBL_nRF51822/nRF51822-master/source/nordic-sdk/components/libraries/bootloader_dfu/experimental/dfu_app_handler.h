/*
 * Copyright (c) Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 *   3. Neither the name of Nordic Semiconductor ASA nor the names of other
 *   contributors to this software may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
  
/** @file
 *
 * @defgroup nrf_dfu_app_handler DFU BLE packet handling in Application
 * @{
 *
 * @brief DFU BLE packet handling for application.
 *
 * @details This module implements handling of DFU packets transmitted over BLE for switching from 
 *          application mode to Bootloader running full DFU service.
 *          This module only handles the StartDFU packet allowing for any BLE application to expose
 *          support for the DFU service.
 *          Actual DFU service will execute in dedicated environment after a BLE disconnect and 
 *          reset of the nRF51 chip.
 *          The host must then reconnect and can continue the update procedure with access to full
 *          DFU service.
 *
 * @note The application must propagate dfu events to the DFU App handler module by calling
 *       dfu_app_on_dfu_evt() from the @ref ble_dfu_evt_handler_t callback.
 */
 
#ifndef DFU_APP_HANDLER_H__
#define DFU_APP_HANDLER_H__

#include "ble_dfu.h"
#include "nrf_svc.h"
#include "bootloader_types.h"
#include "device_manager.h"

/**@brief DFU Application reset prepare function. This function is a callback which allows the 
 *        application to prepare for an upcoming application reset. 
 */
typedef void (*dfu_app_reset_prepare_t)(void);

/**@brief   Function for handling of \ref ble_dfu_evt_t from DFU Service. 
 *
 * @details The application must inject this function into the DFU service or propagate DFU events
 *          to dfu_app_handler module by calling this function in application specific DFU event 
 *          handler.
 * 
 * @param[in] p_dfu              Pointer to the DFU Service structure for which the include event 
 *                               relates.
 * @param[in] p_evt  Pointer to the DFU event.
 */
void dfu_app_on_dfu_evt(ble_dfu_t * p_dfu, ble_dfu_evt_t * p_evt);

/**@brief Function for registering for reset prepare calls. 
 *
 * @details The function provided will be executed before reseting the system into Bootloader/DFU
 *          mode. By setting this function the caller will be notified prior to the reset and can
 *          thus prepare the application for reset. As example the application can gracefully
 *          disconnect any peers on BLE, turning of LEDS, ensure all pending flash operations
 *          has completed, etc.
 *
 * @param[in] reset_prepare_func  Function to be execute prior to a reset.
 */
void dfu_app_reset_prepare_set(dfu_app_reset_prepare_t reset_prepare_func);

/**@brief Function for setting Device manager handle for current BLE connection.
 *
 * @param[in] p_dm_handle  Pointer to device manager handle of current connection.
 */
void dfu_app_set_dm_handle(dm_handle_t const * p_dm_handle);

#endif // DFU_APP_HANDLER_H__

/** @} */
