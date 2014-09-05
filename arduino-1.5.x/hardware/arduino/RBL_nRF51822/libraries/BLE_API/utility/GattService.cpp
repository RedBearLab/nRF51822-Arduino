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


#include <stdio.h>
#include <string.h>

#include "GattService.h"

/**************************************************************************/
/*!
    @brief  Creates a new GattService using the specified 128-bit UUID

    @note   The UUID value must be unique on the device

    @param[in]  uuid
                The 16 byte (128-bit) UUID to use for this characteristic

    @section EXAMPLE

    @code

    @endcode
*/
/**************************************************************************/
GattService::GattService(const UUID &uuid, GattCharacteristic *characteristics[], unsigned numCharacteristics) :
    _primaryServiceID(uuid), _characteristicCount(numCharacteristics), _characteristics(characteristics), _handle(0)
{
    /* empty */
}
