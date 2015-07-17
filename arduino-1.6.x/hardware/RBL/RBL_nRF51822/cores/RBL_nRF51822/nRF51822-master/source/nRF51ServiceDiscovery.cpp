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
#include "nRF51ServiceDiscovery.h"

extern Serial pc;

ble_error_t
nRF51ServiceDiscovery::launchCharacteristicDiscovery(Gap::Handle_t connectionHandle,
                                                   Gap::Handle_t startHandle,
                                                   Gap::Handle_t endHandle)
{
    characteristicDiscoveryStarted(connectionHandle);

    ble_gattc_handle_range_t handleRange = {
        .start_handle = startHandle,
        .end_handle   = endHandle
    };
    uint32_t rc;
    if ((rc = sd_ble_gattc_characteristics_discover(connectionHandle, &handleRange)) != NRF_SUCCESS) {
        terminateCharacteristicDiscovery();
        switch (rc) {
            case BLE_ERROR_INVALID_CONN_HANDLE:
            case NRF_ERROR_INVALID_ADDR:
                return BLE_ERROR_INVALID_PARAM;
            case NRF_ERROR_BUSY:
                return BLE_STACK_BUSY;
            default:
            case NRF_ERROR_INVALID_STATE:
                return BLE_ERROR_INVALID_STATE;
        }
    }

    return BLE_ERROR_NONE;
}

void
nRF51ServiceDiscovery::setupDiscoveredServices(const ble_gattc_evt_prim_srvc_disc_rsp_t *response)
{
    serviceIndex = 0;
    numServices  = response->count;
	
	pc.printf("setupDiscoveredServices \r\n");  
    /* Account for the limitation on the number of discovered services we can handle at a time. */
    if (numServices > BLE_DB_DISCOVERY_MAX_SRV) {
        numServices = BLE_DB_DISCOVERY_MAX_SRV;
    }

    serviceUUIDDiscoveryQueue.reset();
    for (unsigned serviceIndex = 0; serviceIndex < numServices; serviceIndex++) {
        if (response->services[serviceIndex].uuid.type == BLE_UUID_TYPE_UNKNOWN) {
            pc.printf("BLE_UUID_TYPE_UNKNOWN \r\n");  
			serviceUUIDDiscoveryQueue.enqueue(serviceIndex);
            services[serviceIndex].setup(response->services[serviceIndex].handle_range.start_handle,
                                         response->services[serviceIndex].handle_range.end_handle);
        } else {
            services[serviceIndex].setup(response->services[serviceIndex].uuid.uuid,
                                         response->services[serviceIndex].handle_range.start_handle,
                                         response->services[serviceIndex].handle_range.end_handle);
			pc.printf("KNOWN \r\n");  
		}
    }

    /* Trigger discovery of service UUID if necessary. */
    if (serviceUUIDDiscoveryQueue.getCount()) {
        serviceUUIDDiscoveryQueue.triggerFirst();
    }
	pc.printf("setupDiscoveredServices end//\r\n");  
}

void
nRF51ServiceDiscovery::setupDiscoveredCharacteristics(const ble_gattc_evt_char_disc_rsp_t *response)
{
    characteristicIndex = 0;
    numCharacteristics  = response->count;

	pc.printf("setupDiscoveredCharacteristics \r\n");
    /* Account for the limitation on the number of discovered characteristics we can handle at a time. */
    if (numCharacteristics > BLE_DB_DISCOVERY_MAX_CHAR_PER_SRV) {
        numCharacteristics = BLE_DB_DISCOVERY_MAX_CHAR_PER_SRV;
    }

    charUUIDDiscoveryQueue.reset();
    for (unsigned charIndex = 0; charIndex < numCharacteristics; charIndex++) {
        if (response->chars[charIndex].uuid.type == BLE_UUID_TYPE_UNKNOWN) {
			pc.printf("BLE_UUID_TYPE_UNKNOWN \r\n");
            charUUIDDiscoveryQueue.enqueue(charIndex);
            characteristics[charIndex].setup(gattc,
                                             connHandle,
                                             response->chars[charIndex].char_props,
                                             response->chars[charIndex].handle_decl,
                                             response->chars[charIndex].handle_value);
        } else {
            characteristics[charIndex].setup(gattc,
                                             connHandle,
                                             response->chars[charIndex].uuid.uuid,
                                             response->chars[charIndex].char_props,
                                             response->chars[charIndex].handle_decl,
                                             response->chars[charIndex].handle_value);
			pc.printf("KNOWN \r\n");  
        }
    }

    /* Trigger discovery of char UUID if necessary. */
    if (charUUIDDiscoveryQueue.getCount()) {
        charUUIDDiscoveryQueue.triggerFirst();
    }
	pc.printf("setupDiscoveredCharacteristics end//\r\n");  
}

void
nRF51ServiceDiscovery::progressCharacteristicDiscovery(void)
{
    /* Iterate through the previously discovered characteristics cached in characteristics[]. */
    // while ((state == CHARACTERISTIC_DISCOVERY_ACTIVE) && (characteristicIndex < numCharacteristics)) {
		// pc.printf("progressCharacteristicDiscovery \r\n");  
		// pc.printf("matchingCharacteristicUUID : %2x \r\n", matchingCharacteristicUUID.getShortUUID());
		// pc.printf("characteristics[characteristicIndex].getShortUUID() : %2x \r\n", characteristics[characteristicIndex].getShortUUID());
		// pc.printf("matchingServiceUUID.getShortUUID() : %2x \r\n", matchingServiceUUID.getShortUUID() );
		// pc.printf("UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN) : %2x \r\n", UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN) );
        // if ((matchingCharacteristicUUID.getShortUUID() == UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN)) ||
            // ((matchingCharacteristicUUID.getShortUUID() == characteristics[characteristicIndex].getShortUUID()) &&
             // (matchingServiceUUID.getShortUUID() != UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN)))) {
			// pc.printf("progressCharacteristic1 \r\n");  
            // if (characteristicCallback) {
                // characteristicCallback(&characteristics[characteristicIndex]);
            // }
        // }

        // characteristicIndex++;
    // }
	// while( (state == CHARACTERISTIC_DISCOVERY_ACTIVE) && (characteristicIndex < numCharacteristics) ) {
		// pc.printf("progressCharacteristicDiscovery \r\n");  
		// if( (matchingCharacteristicUUID.shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_SHORT)) &&
			// (characteristics[characteristicIndex].getUUID().shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_SHORT)) &&
			// (matchingCharacteristicUUID.getShortUUID() == characteristics[characteristicIndex].getUUID().getShortUUID()) )
		// {
			// pc.printf("Short uuid \r\n");  
            // if (characteristicCallback) {
                // characteristicCallback(&characteristics[characteristicIndex]);
            // }	
		// }
		// else if( (matchingCharacteristicUUID.shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_LONG)) &&
			     // (characteristics[characteristicIndex].getUUID().shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_LONG)) &&
				 // (0x00 == memcmp(matchingCharacteristicUUID.getBaseUUID(), characteristics[characteristicIndex].getUUID().getBaseUUID(), UUID::LENGTH_OF_LONG_UUID)) )
		// {
			// pc.printf("Long uuid \r\n");  
            // if (characteristicCallback) {
                // characteristicCallback(&characteristics[characteristicIndex]);
            // }				
		// }
		
		// characteristicIndex++;
	// }
	while( (state == CHARACTERISTIC_DISCOVERY_ACTIVE) && (characteristicIndex < numCharacteristics) ) {
		pc.printf("progressCharacteristicDiscovery \r\n");  
		if( ((matchingCharacteristicUUID.shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_SHORT)) &&
			 (characteristics[characteristicIndex].getUUID().shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_SHORT)) &&
			 (matchingCharacteristicUUID.getShortUUID() == characteristics[characteristicIndex].getUUID().getShortUUID()) 
			) || 
			((matchingCharacteristicUUID.shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_LONG)) &&
			 (characteristics[characteristicIndex].getUUID().shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_LONG)) &&
			 (0x00 == memcmp(matchingCharacteristicUUID.getBaseUUID(), characteristics[characteristicIndex].getUUID().getBaseUUID(), UUID::LENGTH_OF_LONG_UUID))
			) )
		{
			pc.printf("match uuid \r\n");  
            if (characteristicCallback) {
                characteristicCallback(&characteristics[characteristicIndex]);
            }	
		}
		
		characteristicIndex++;
	}
    /* Relaunch discovery of new characteristics beyond the last entry cached in characteristics[]. */
    if (state == CHARACTERISTIC_DISCOVERY_ACTIVE) {
        /* Determine the ending handle of the last cached characteristic. */
        Gap::Handle_t startHandle = characteristics[characteristicIndex - 1].getValueHandle() + 1;
        Gap::Handle_t endHandle   = services[serviceIndex].getEndHandle();
        resetDiscoveredCharacteristics(); /* Note: resetDiscoveredCharacteristics() must come after fetching start and end Handles. */
		pc.printf("check handle \r\n");  
        if (startHandle < endHandle) {
            ble_gattc_handle_range_t handleRange = {
                .start_handle = startHandle,
                .end_handle   = endHandle
            };
            if (sd_ble_gattc_characteristics_discover(connHandle, &handleRange) != NRF_SUCCESS) {
                terminateCharacteristicDiscovery();
            }
        } else {
			pc.printf("terminateCharacteristicDiscovery \r\n");  
            terminateCharacteristicDiscovery();
        }
    }
	pc.printf("progressCharacteristicDiscovery end//\r\n");  
}

void
nRF51ServiceDiscovery::progressServiceDiscovery(void)
{
    /* Iterate through the previously discovered services cached in services[]. */
    // while ((state == SERVICE_DISCOVERY_ACTIVE) && (serviceIndex < numServices)) {
		// pc.printf("progressServiceDiscovery \r\n");  
        // if ((matchingServiceUUID.getShortUUID() == UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN)) ||
            // (matchingServiceUUID.getShortUUID() == services[serviceIndex].getUUID().getShortUUID())) {
			// pc.printf("progressService1 \r\n");  
            // if (serviceCallback && (matchingCharacteristicUUID.getShortUUID() == UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN))) {
				// pc.printf("progressService2 \r\n");  
                // serviceCallback(&services[serviceIndex]);
            // }

            // if ((state == SERVICE_DISCOVERY_ACTIVE) && characteristicCallback) {
				// pc.printf("progressService3 \r\n");  
                // launchCharacteristicDiscovery(connHandle, services[serviceIndex].getStartHandle(), services[serviceIndex].getEndHandle());
            // } else {
                // serviceIndex++;
            // }
        // } else {
            // serviceIndex++;
        // }
    // }
	// while ((state == SERVICE_DISCOVERY_ACTIVE) && (serviceIndex < numServices)) {
		// pc.printf("progressServiceDiscovery \r\n"); 
		// if( (matchingServiceUUID.shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_SHORT)) && 
			// (services[serviceIndex].getUUID().shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_SHORT)) &&
			// (matchingServiceUUID.getShortUUID() == services[serviceIndex].getUUID().getShortUUID()) )
		// {
			// pc.printf("Short type \r\n"); 
			// if ( serviceCallback ){
				// pc.printf("CallBack \r\n");  
                // serviceCallback(&services[serviceIndex]);
            // }
			
			// if( (state == SERVICE_DISCOVERY_ACTIVE) && characteristicCallback )
			// {
				// pc.printf("start Scan chars \r\n");  
                // launchCharacteristicDiscovery(connHandle, services[serviceIndex].getStartHandle(), services[serviceIndex].getEndHandle());		
			// }
			// else
			// {
				// serviceIndex++;
			// }
		// }
		// else if( (matchingServiceUUID.shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_LONG)) &&
				 // (services[serviceIndex].getUUID().shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_LONG)) &&
				 // (0x00 == memcmp(matchingServiceUUID.getBaseUUID(), services[serviceIndex].getUUID().getBaseUUID(), UUID::LENGTH_OF_LONG_UUID)) )
		// {
			// pc.printf("Long type \r\n"); 
			// if ( serviceCallback ){
				// pc.printf("CallBack \r\n");  
                // serviceCallback(&services[serviceIndex]);
            // }
			
			// if( (state == SERVICE_DISCOVERY_ACTIVE) && characteristicCallback )
			// {
				// pc.printf("start Scan chars \r\n");  
                // launchCharacteristicDiscovery(connHandle, services[serviceIndex].getStartHandle(), services[serviceIndex].getEndHandle());		
			// }	
			// else
			// {
				// serviceIndex++;
			// }
		// }
		// else
		// {
			// serviceIndex++;
		// }
	// }

	while ((state == SERVICE_DISCOVERY_ACTIVE) && (serviceIndex < numServices)) {
		pc.printf("progressServiceDiscovery \r\n"); 
		if( ((matchingServiceUUID.shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_SHORT)) && 
			 (services[serviceIndex].getUUID().shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_SHORT)) &&
			 (matchingServiceUUID.getShortUUID() == services[serviceIndex].getUUID().getShortUUID()) 
			) || 
			((matchingServiceUUID.shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_LONG)) &&
			 (services[serviceIndex].getUUID().shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_LONG)) &&
			 (0x00 == memcmp(matchingServiceUUID.getBaseUUID(), services[serviceIndex].getUUID().getBaseUUID(), UUID::LENGTH_OF_LONG_UUID))
			) )
		{
			pc.printf("match uuid \r\n"); 
			if ( serviceCallback ){
				pc.printf("CallBack \r\n");  
                serviceCallback(&services[serviceIndex]);
            }
			
			if( (state == SERVICE_DISCOVERY_ACTIVE) && characteristicCallback )
			{
				pc.printf("start Scan chars \r\n");  
                launchCharacteristicDiscovery(connHandle, services[serviceIndex].getStartHandle(), services[serviceIndex].getEndHandle());		
			}
			else
			{
				serviceIndex++;
			}
		}
		else
		{
			serviceIndex++;
		}
	}
    /* Relaunch discovery of new services beyond the last entry cached in services[]. */
    if ((state == SERVICE_DISCOVERY_ACTIVE) && (numServices > 0) && (serviceIndex > 0)) {
        /* Determine the ending handle of the last cached service. */
        Gap::Handle_t endHandle = services[serviceIndex - 1].getEndHandle();
        resetDiscoveredServices(); /* Note: resetDiscoveredServices() must come after fetching endHandle. */
		pc.printf("progressService4 \r\n");  
        if (endHandle == SRV_DISC_END_HANDLE) {
			pc.printf("terminateServiceDiscovery \r\n");  
            terminateServiceDiscovery();
        } else {
            if (sd_ble_gattc_primary_services_discover(connHandle, endHandle, NULL) != NRF_SUCCESS) { 
                terminateServiceDiscovery();
            }
        }
    }
	pc.printf("progressServiceDiscovery end//\r\n");  
}

void
nRF51ServiceDiscovery::ServiceUUIDDiscoveryQueue::triggerFirst(void)
{
	pc.printf("ServiceUUIDDiscoveryQueue::triggerFirst \r\n");  
    while (numIndices) { /* loop until a call to char_value_by_uuid_read() succeeds or we run out of pending indices. */
        parentDiscoveryObject->state = DISCOVER_SERVICE_UUIDS;

        unsigned serviceIndex = getFirst();
        ble_uuid_t uuid = {
            .uuid = BLE_UUID_SERVICE_PRIMARY,
            .type = BLE_UUID_TYPE_BLE,
        };
        ble_gattc_handle_range_t handleRange = {
            .start_handle = parentDiscoveryObject->services[serviceIndex].getStartHandle(),
            .end_handle   = parentDiscoveryObject->services[serviceIndex].getEndHandle(),
        };
		pc.printf("ServiceUUIDDiscoveryQueue 1 \r\n");  
        if (sd_ble_gattc_char_value_by_uuid_read(parentDiscoveryObject->connHandle, &uuid, &handleRange) == NRF_SUCCESS) {
            return;
        }

        /* Skip this service if we fail to launch a read for its service-declaration
         * attribute. Its UUID will remain INVALID, and it may not match any filters. */
        dequeue();
    }

    /* Switch back to service discovery upon exhausting the service-indices pending UUID discovery. */
    if (parentDiscoveryObject->state == DISCOVER_SERVICE_UUIDS) {
		pc.printf("ServiceUUIDDiscoveryQueue 2 \r\n");  
        parentDiscoveryObject->state = SERVICE_DISCOVERY_ACTIVE;
    }
	pc.printf("ServiceUUIDDiscoveryQueue::triggerFirst end//\r\n");
}

void
nRF51ServiceDiscovery::CharUUIDDiscoveryQueue::triggerFirst(void)
{
	pc.printf("CharUUIDDiscoveryQueue::triggerFirst \r\n");  
    while (numIndices) { /* loop until a call to char_value_by_uuid_read() succeeds or we run out of pending indices. */
        parentDiscoveryObject->state = DISCOVER_CHARACTERISTIC_UUIDS;

        unsigned charIndex = getFirst();
        ble_uuid_t uuid = {
            .uuid = BLE_UUID_CHARACTERISTIC,
            .type = BLE_UUID_TYPE_BLE,
        };
        ble_gattc_handle_range_t handleRange = { };
        handleRange.start_handle = parentDiscoveryObject->characteristics[charIndex].getDeclHandle();
        handleRange.end_handle   = parentDiscoveryObject->characteristics[charIndex].getDeclHandle() + 1;
		pc.printf("CharUUIDDiscoveryQueue 1 \r\n");  
        if (sd_ble_gattc_char_value_by_uuid_read(parentDiscoveryObject->connHandle, &uuid, &handleRange) == NRF_SUCCESS) {
            return;
        }

        /* Skip this service if we fail to launch a read for its service-declaration
         * attribute. Its UUID will remain INVALID, and it may not match any filters. */
        dequeue();
    }

    /* Switch back to service discovery upon exhausting the service-indices pending UUID discovery. */
    if (parentDiscoveryObject->state == DISCOVER_CHARACTERISTIC_UUIDS) {
		pc.printf("CharUUIDDiscoveryQueue 2 \r\n");  
        parentDiscoveryObject->state = CHARACTERISTIC_DISCOVERY_ACTIVE;
    }
	pc.printf("CharUUIDDiscoveryQueue::triggerFirst end//\r\n");
}

void
nRF51ServiceDiscovery::processDiscoverUUIDResponse(const ble_gattc_evt_char_val_by_uuid_read_rsp_t *response)
{
	pc.printf("processDiscoverUUIDResponse \r\n");  
    if (state == DISCOVER_SERVICE_UUIDS) {
		pc.printf("DISCOVER_SERVICE_UUIDS \r\n");  
        if ((response->count == 1) && (response->value_len == UUID::LENGTH_OF_LONG_UUID)) {
            UUID::LongUUIDBytes_t uuid;
            /* Switch longUUID bytes to MSB */
			pc.printf("DISCOVER_SERVICE_UUIDS is : "); 
            for (unsigned i = 0; i < UUID::LENGTH_OF_LONG_UUID; i++) {
                uuid[i] = response->handle_value[0].p_value[UUID::LENGTH_OF_LONG_UUID - 1 - i];
				pc.printf("%x ", uuid[i]); 
            }
			pc.printf("\r\n"); 
			pc.printf("DISCOVER_SERVICE_UUIDS 1 \r\n");  
            unsigned serviceIndex = serviceUUIDDiscoveryQueue.dequeue();
            services[serviceIndex].setupLongUUID(uuid);

            serviceUUIDDiscoveryQueue.triggerFirst();
        } else {
			pc.printf("DISCOVER_SERVICE_UUIDS 2 \r\n");  
            serviceUUIDDiscoveryQueue.dequeue();
        }
    } else if (state == DISCOVER_CHARACTERISTIC_UUIDS) {
		pc.printf("DISCOVER_CHARACTERISTIC_UUIDS \r\n");  
        if ((response->count == 1) && (response->value_len == UUID::LENGTH_OF_LONG_UUID + 1 /* props */ + 2 /* value handle */)) {
            UUID::LongUUIDBytes_t uuid;
            /* Switch longUUID bytes to MSB */
			pc.printf("DISCOVER_CHARACTERISTIC_UUIDS is : "); 
            for (unsigned i = 0; i < UUID::LENGTH_OF_LONG_UUID; i++) {
                uuid[i] = response->handle_value[0].p_value[3 + UUID::LENGTH_OF_LONG_UUID - 1 - i];
				pc.printf("%x ", uuid[i]); 
            }
			pc.printf("\r\n");
			pc.printf("DISCOVER_CHARACTERISTIC_UUIDS 1\r\n");  
            unsigned charIndex = charUUIDDiscoveryQueue.dequeue();
            characteristics[charIndex].setupLongUUID(uuid);

            charUUIDDiscoveryQueue.triggerFirst();
        } else {
			pc.printf("DISCOVER_CHARACTERISTIC_UUIDS 2\r\n");  
            charUUIDDiscoveryQueue.dequeue();
        }
    }
	pc.printf("processDiscoverUUIDResponse end//\r\n");
}
