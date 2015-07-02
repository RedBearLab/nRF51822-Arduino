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

#ifndef __NRF51822_H__
#define __NRF51822_H__

#include "mbed.h"
#include "ble/blecommon.h"
#include "ble/BLE.h"
#include "nRF51Gap.h"
#include "nRF51GattServer.h"
#include "nRF51GattClient.h"
#include "nRF51SecurityManager.h"
#include "btle.h"

class nRF51822n : public BLEInstanceBase
{
public:
    nRF51822n(void);
    virtual ~nRF51822n(void);

    virtual ble_error_t init(void);
    virtual ble_error_t shutdown(void);
    virtual const char *getVersion(void);

    virtual Gap &getGap() {
        return nRF51Gap::getInstance();
    };
    virtual const Gap &getGap() const  {
        return nRF51Gap::getInstance();
    };
    virtual GattServer &getGattServer() {
        return nRF51GattServer::getInstance();
    };
    virtual const GattServer &getGattServer() const {
        return nRF51GattServer::getInstance();
    };
    virtual GattClient &getGattClient() {
        return nRF51GattClient::getInstance();
    }
    virtual const SecurityManager &getSecurityManager() const {
        return nRF51SecurityManager::getInstance();
    }
    virtual SecurityManager &getSecurityManager() {
        return nRF51SecurityManager::getInstance();
    }
    virtual void        waitForEvent(void);
};

#endif
