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

#include "ble/BLE.h"

#if defined(TARGET_OTA_ENABLED)
#include "ble/services/DFUService.h"
#endif

ble_error_t
BLE::init()
{
    ble_error_t err = transport->init();
    if (err != BLE_ERROR_NONE) {
        return err;
    }

    /* Platforms enabled for DFU should introduce the DFU Service into
     * applications automatically. */
#if defined(TARGET_OTA_ENABLED)
    static DFUService dfu(*this); // defined static so that the object remains alive
#endif // TARGET_OTA_ENABLED

    return BLE_ERROR_NONE;
}
