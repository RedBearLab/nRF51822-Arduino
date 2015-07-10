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

#ifndef _BTLE_SECURITY_H_
#define _BTLE_SECURITY_H_

#include "ble/Gap.h"
#include "ble/SecurityManager.h"

/**
 * Enable Nordic's Device Manager, which brings in functionality from the
 * stack's Security Manager. The Security Manager implements the actual
 * cryptographic algorithms and protocol exchanges that allow two devices to
 * securely exchange data and privately detect each other.
 *
 * @param[in]  enableBonding Allow for bonding.
 * @param[in]  requireMITM   Require protection for man-in-the-middle attacks.
 * @param[in]  iocaps        To specify IO capabilities of this peripheral,
 *                           such as availability of a display or keyboard to
 *                           support out-of-band exchanges of security data.
 * @param[in]  passkey       To specify a static passkey.
 *
 * @return BLE_ERROR_NONE on success.
 */
ble_error_t btle_initializeSecurity(bool                                      enableBonding = true,
                                    bool                                      requireMITM   = true,
                                    SecurityManager::SecurityIOCapabilities_t iocaps        = SecurityManager::IO_CAPS_NONE,
                                    const SecurityManager::Passkey_t          passkey       = NULL);

/**
 * Get the security status of a link.
 *
 * @param[in]  connectionHandle
 *               Handle to identify the connection.
 * @param[out] securityStatusP
 *               security status.
 *
 * @return BLE_SUCCESS Or appropriate error code indicating reason for failure.
 */
ble_error_t btle_getLinkSecurity(Gap::Handle_t connectionHandle, SecurityManager::LinkSecurityStatus_t *securityStatusP);

/**
 * Function for deleting all peer device context and all related bonding
 * information from the database.
 *
 * @retval BLE_ERROR_NONE             On success, else an error code indicating reason for failure.
 * @retval BLE_ERROR_INVALID_STATE    If the API is called without module initialization and/or
 *                                    application registration.
 */
ble_error_t btle_purgeAllBondingState(void);

#endif /* _BTLE_SECURITY_H_ */
