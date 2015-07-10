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

#include "mbed.h"
#include "nRF51822n.h"
#include "nrf_soc.h"

#include "btle/btle.h"
#include "nrf_delay.h"

#include "softdevice_handler.h"

/**
 * The singleton which represents the nRF51822 transport for the BLE.
 */
static nRF51822n deviceInstance;

/**
 * BLE-API requires an implementation of the following function in order to
 * obtain its transport handle.
 */
BLEInstanceBase *
createBLEInstance(void)
{
    return (&deviceInstance);
}

nRF51822n::nRF51822n(void)
{
}

nRF51822n::~nRF51822n(void)
{
}

const char *nRF51822n::getVersion(void)
{
    static char versionString[32];
    static bool versionFetched = false;

    if (!versionFetched) {
        ble_version_t version;
        if ((sd_ble_version_get(&version) == NRF_SUCCESS) && (version.company_id == 0x0059)) {
            switch (version.version_number) {
                case 0x07:
                    snprintf(versionString, sizeof(versionString), "Nordic BLE4.1 fw:%04x", version.subversion_number);
                    break;
                default:
                    snprintf(versionString, sizeof(versionString), "Nordic (spec unknown) fw:%04x", version.subversion_number);
                    break;
            }
            versionFetched = true;
        } else {
            strncpy(versionString, "unknown", sizeof(versionString));
        }
    }

    return versionString;
}

ble_error_t nRF51822n::init(void)
{
    /* ToDo: Clear memory contents, reset the SD, etc. */
    btle_init();

    return BLE_ERROR_NONE;
}

ble_error_t nRF51822n::shutdown(void)
{
    return (softdevice_handler_sd_disable() == NRF_SUCCESS) ? BLE_ERROR_NONE : BLE_STACK_BUSY;
}

void
nRF51822n::waitForEvent(void)
{
    sd_app_evt_wait();
}
