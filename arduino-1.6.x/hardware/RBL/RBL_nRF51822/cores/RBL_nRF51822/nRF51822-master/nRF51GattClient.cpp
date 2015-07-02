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

#include "nRF51GattClient.h"

nRF51GattClient nRFGattClientSingleton;

nRF51GattClient &
nRF51GattClient::getInstance(void) {
    return nRFGattClientSingleton;
}

ble_error_t
nRF51GattClient::launchServiceDiscovery(Gap::Handle_t                               connectionHandle,
                                        ServiceDiscovery::ServiceCallback_t         sc,
                                        ServiceDiscovery::CharacteristicCallback_t  cc,
                                        const UUID                                 &matchingServiceUUIDIn,
                                        const UUID                                 &matchingCharacteristicUUIDIn)
{
    return discovery.launch(connectionHandle, sc, cc, matchingServiceUUIDIn, matchingCharacteristicUUIDIn);
}
