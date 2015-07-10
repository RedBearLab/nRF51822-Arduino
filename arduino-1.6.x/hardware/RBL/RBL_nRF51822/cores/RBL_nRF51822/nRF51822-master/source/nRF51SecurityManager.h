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

#ifndef __NRF51822_SECURITY_MANAGER_H__
#define __NRF51822_SECURITY_MANAGER_H__

#include <stddef.h>

#include "ble/SecurityManager.h"
#include "btle_security.h"

class nRF51SecurityManager : public SecurityManager
{
public:
    static nRF51SecurityManager &getInstance();

    /* Functions that must be implemented from SecurityManager */
    virtual ble_error_t init(bool                     enableBonding,
                             bool                     requireMITM,
                             SecurityIOCapabilities_t iocaps,
                             const Passkey_t          passkey) {
        return btle_initializeSecurity(enableBonding, requireMITM, iocaps, passkey);
    }

    virtual ble_error_t getLinkSecurity(Gap::Handle_t connectionHandle, LinkSecurityStatus_t *securityStatusP) {
        return btle_getLinkSecurity(connectionHandle, securityStatusP);
    }

    virtual ble_error_t purgeAllBondingState(void) {
        return btle_purgeAllBondingState();
    }

public:
    nRF51SecurityManager() {
        /* empty */
    }

private:
    nRF51SecurityManager(const nRF51SecurityManager &);
    const nRF51SecurityManager& operator=(const nRF51SecurityManager &);
};

#endif // ifndef __NRF51822_SECURITY_MANAGER_H__
