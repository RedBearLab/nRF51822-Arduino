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

#ifndef __DISCOVERED_SERVICE_H__
#define __DISCOVERED_SERVICE_H__

#include "UUID.h"
#include "GattAttribute.h"

/**@brief Type for holding information about the service and the characteristics found during
 *        the discovery process.
 */
class DiscoveredService {
public:
     void setup(UUID uuidIn, GattAttribute::Handle_t startHandleIn, GattAttribute::Handle_t endHandleIn) {
         uuid        = uuidIn;
         startHandle = startHandleIn;
         endHandle   = endHandleIn;
     }

     void setup(GattAttribute::Handle_t startHandleIn, GattAttribute::Handle_t endHandleIn) {
         startHandle = startHandleIn;
         endHandle   = endHandleIn;
     }

    void setupLongUUID(UUID::LongUUIDBytes_t longUUID) {
         uuid.setupLong(longUUID);
    }

public:
    const UUID &getUUID(void) const {
        return uuid;
    }

    const GattAttribute::Handle_t& getStartHandle(void) const {
        return startHandle;
    }
    const GattAttribute::Handle_t& getEndHandle(void) const {
        return endHandle;
    }

public:
    DiscoveredService() : uuid(UUID::ShortUUIDBytes_t(0)),
                          startHandle(GattAttribute::INVALID_HANDLE),
                          endHandle(GattAttribute::INVALID_HANDLE) {
        /* empty */
    }

private:
    DiscoveredService(const DiscoveredService &);

private:
    UUID                    uuid;        /**< UUID of the service.  */
    GattAttribute::Handle_t startHandle; /**< Service Handle Range. */
    GattAttribute::Handle_t endHandle;   /**< Service Handle Range. */
};

#endif /*__DISCOVERED_SERVICE_H__*/
