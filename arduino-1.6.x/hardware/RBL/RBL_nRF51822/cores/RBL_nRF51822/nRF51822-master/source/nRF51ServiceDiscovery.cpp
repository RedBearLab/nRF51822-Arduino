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

//extern Serial pc;

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
    unsigned response_Index = 0;
	unsigned pre_numServices = numServices;
    numServices  = numServices + response->count;
	
	//pc.printf("setupDiscoveredServices \r\n");  
    /* Account for the limitation on the number of discovered services we can handle at a time. */
    if (numServices > BLE_DB_DISCOVERY_MAX_SRV) {
		//pc.printf("Not enough memory \r\n");
        numServices = BLE_DB_DISCOVERY_MAX_SRV;
    }

    serviceUUIDDiscoveryQueue.reset();
    for (unsigned Index = pre_numServices; Index < numServices; Index++,response_Index++) {
        if (response->services[response_Index].uuid.type == BLE_UUID_TYPE_UNKNOWN) {
            //pc.printf("BLE_UUID_TYPE_UNKNOWN \r\n");  
			serviceUUIDDiscoveryQueue.enqueue(Index);
            services[Index].setup(response->services[response_Index].handle_range.start_handle,
                                         response->services[response_Index].handle_range.end_handle);
										 
			//pc.printf("startHandle : %2x \r\n", response->services[response_Index].handle_range.start_handle);
			//pc.printf("endHandle : %2x \r\n", response->services[response_Index].handle_range.end_handle);
        } else {
            services[Index].setup(response->services[response_Index].uuid.uuid,
                                         response->services[response_Index].handle_range.start_handle,
                                         response->services[response_Index].handle_range.end_handle);
			//pc.printf("KNOWN \r\n");  
		}
    }

    /* Trigger discovery of service UUID if necessary. */
    if (serviceUUIDDiscoveryQueue.getCount()) {
        serviceUUIDDiscoveryQueue.triggerFirst();
    }
	//pc.printf("setupDiscoveredServices end//\r\n");  
}

void
nRF51ServiceDiscovery::setupDiscoveredCharacteristics(const ble_gattc_evt_char_disc_rsp_t *response)
{
	unsigned response_Index = 0;
	unsigned pre_numChars = numCharacteristics;
    numCharacteristics  = numCharacteristics + response->count;

	//pc.printf("setupDiscoveredCharacteristics \r\n");
	//pc.printf("Characteristics count %d \r\n", response->count);
    /* Account for the limitation on the number of discovered characteristics we can handle at a time. */
    if (numCharacteristics > BLE_DB_DISCOVERY_MAX_CHAR_PER_SRV) {
        numCharacteristics = BLE_DB_DISCOVERY_MAX_CHAR_PER_SRV;
    }

    charUUIDDiscoveryQueue.reset();
    for (unsigned charIndex = pre_numChars; charIndex < numCharacteristics; charIndex++,response_Index++) {
        if (response->chars[response_Index].uuid.type == BLE_UUID_TYPE_UNKNOWN) {
			//pc.printf("BLE_UUID_TYPE_UNKNOWN \r\n");
            charUUIDDiscoveryQueue.enqueue(charIndex);
            characteristics[charIndex].setup(gattc,
                                             connHandle,
                                             response->chars[response_Index].char_props,
                                             response->chars[response_Index].handle_decl,
                                             response->chars[response_Index].handle_value);
        } else {
            characteristics[charIndex].setup(gattc,
                                             connHandle,
                                             response->chars[response_Index].uuid.uuid,
                                             response->chars[response_Index].char_props,
                                             response->chars[response_Index].handle_decl,
                                             response->chars[response_Index].handle_value);
			//pc.printf("KNOWN \r\n");  
        }
    }

    /* Trigger discovery of char UUID if necessary. */
    if (charUUIDDiscoveryQueue.getCount()) {
        charUUIDDiscoveryQueue.triggerFirst();
    }
	//pc.printf("setupDiscoveredCharacteristics end//\r\n");  
}

void 
nRF51ServiceDiscovery::setupDiscoveredDescriptor(const ble_gattc_evt_desc_disc_rsp_t *respone)
{
	if(state == DESCRIPTOR_DISCOVERY_ACTIVE )
	{	//pc.printf("setupDiscoveredDescriptor \r\n");
		for(uint8_t index=0; index<respone->count; index++)
		{	//pc.printf("count \r\n");
			if(respone->descs[index].uuid.uuid ==  BLE_UUID_DESCRIPTOR_CHAR_USER_DESC)
			{	//pc.printf("USER_DESC \r\n");
				characteristics[matchCharsIndex].setDescHandle(respone->descs[index].handle);
			}
			
			if(respone->descs[index].uuid.uuid ==  BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG)
			{	//pc.printf("CCCD \r\n");
				characteristics[matchCharsIndex].setCCCDHandle(respone->descs[index].handle);
			}			
		}
		Gap::Handle_t startHandle = respone->descs[respone->count-1].handle + 1;
		Gap::Handle_t endHandle;
		if(matchCharsIndex+1 == numCharacteristics){
			/* it is the last characteristic,so the endHandle is the service endHandle */
			//pc.printf("last \r\n");
			endHandle = services[matchServiceIndex].getEndHandle();
		}
		else{
			/* it is not the last one, so the endHandle is the next characteristic declHandle*/
			//pc.printf("not last \r\n");
			endHandle = characteristics[matchCharsIndex+1].getDeclHandle();
		}

		if(startHandle < endHandle) {
			//pc.printf("restart scan declaration \r\n");
		    ble_gattc_handle_range_t handleRange = {
                .start_handle = startHandle,
                .end_handle   = endHandle
            };
			//pc.printf("startHandle %2x \r\n", startHandle);
			//pc.printf("endHandle %2x \r\n", endHandle);
			uint32_t err_code = sd_ble_gattc_descriptors_discover(connHandle, &handleRange);
			if(err_code != NRF_SUCCESS){
				//pc.printf("err_code : %2x \r\n", err_code);
                terminateDescriptorDiscovery();
            }
		}
		else {
			//pc.printf("terminateDescriptorDiscovery \r\n");
			terminateDescriptorDiscovery();
		}
	}
}

void
nRF51ServiceDiscovery::progressCharacteristicDiscovery(void)
{
	while( (state == CHARACTERISTIC_DISCOVERY_ACTIVE) && (characteristicIndex < numCharacteristics) ) {
		//pc.printf("progressCharacteristicDiscovery \r\n");  
		if( ((matchingCharacteristicUUID.shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_SHORT)) &&
			 (characteristics[characteristicIndex].getUUID().shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_SHORT)) &&
			 (matchingCharacteristicUUID.getShortUUID() == characteristics[characteristicIndex].getUUID().getShortUUID()) 
			) || 
			((matchingCharacteristicUUID.shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_LONG)) &&
			 (characteristics[characteristicIndex].getUUID().shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_LONG)) &&
			 (0x00 == memcmp(matchingCharacteristicUUID.getBaseUUID(), characteristics[characteristicIndex].getUUID().getBaseUUID(), UUID::LENGTH_OF_LONG_UUID))
			) )
		{
			//pc.printf("match uuid \r\n");  
			matchCharsIndex = characteristicIndex;
            //if (characteristicCallback) {
            //    characteristicCallback(&characteristics[characteristicIndex]);
            //}	
		}
		
		characteristicIndex++;
	}
    /* Relaunch discovery of new characteristics beyond the last entry cached in characteristics[]. */
    if (state == CHARACTERISTIC_DISCOVERY_ACTIVE) {
        /* Determine the ending handle of the last cached characteristic. */
        Gap::Handle_t startHandle = characteristics[characteristicIndex - 1].getValueHandle() + 1;
        Gap::Handle_t endHandle   = services[matchServiceIndex].getEndHandle();
        //resetDiscoveredCharacteristics(); /* Note: resetDiscoveredCharacteristics() must come after fetching start and end Handles. */
		//pc.printf("check handle \r\n");  
        if (startHandle < endHandle) {
			//pc.printf("restart scan chars \r\n");  
			//pc.printf("startHandle : %2x \r\n", startHandle);
			//pc.printf("endHandle : %2x \r\n", endHandle);
            ble_gattc_handle_range_t handleRange = {
                .start_handle = startHandle,
                .end_handle   = endHandle
            };
			uint32_t err_code = sd_ble_gattc_characteristics_discover(connHandle, &handleRange);
            //if (sd_ble_gattc_characteristics_discover(connHandle, &handleRange) != NRF_SUCCESS) {
			if(err_code != NRF_SUCCESS){
				//pc.printf("err_code : %2x \r\n", err_code);
                terminateCharacteristicDiscovery();
				//pc.printf("stop \r\n");  
            }
        } else {
			//pc.printf("terminateCharacteristicDiscovery \r\n");  
            terminateCharacteristicDiscovery();
        }
    }
	//pc.printf("progressCharacteristicDiscovery end//\r\n");  
}

void
nRF51ServiceDiscovery::progressServiceDiscovery(void)
{
	while ((state == SERVICE_DISCOVERY_ACTIVE) && (serviceIndex < numServices)) {
		//pc.printf("progressServiceDiscovery \r\n"); 
		if( ((matchingServiceUUID.shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_SHORT)) && 
			 (services[serviceIndex].getUUID().shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_SHORT)) &&
			 (matchingServiceUUID.getShortUUID() == services[serviceIndex].getUUID().getShortUUID()) 
			) || 
			((matchingServiceUUID.shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_LONG)) &&
			 (services[serviceIndex].getUUID().shortOrLong() == UUID::UUID_Type_t(UUID::UUID_TYPE_LONG)) &&
			 (0x00 == memcmp(matchingServiceUUID.getBaseUUID(), services[serviceIndex].getUUID().getBaseUUID(), UUID::LENGTH_OF_LONG_UUID))
			) )
		{
			//pc.printf("match uuid \r\n"); 
			if ( serviceCallback ){
				//pc.printf("CallBack \r\n");  
				matchServiceIndex = serviceIndex;
                serviceCallback(&services[serviceIndex]);
            }
		}
		serviceIndex++;
	}
    /* Relaunch discovery of new services beyond the last entry cached in services[]. */
    if ( (state == SERVICE_DISCOVERY_ACTIVE) ) {
        /* Determine the ending handle of the last cached service. */
        Gap::Handle_t endHandle = services[serviceIndex - 1].getEndHandle();
        //resetDiscoveredServices(); /* Note: resetDiscoveredServices() must come after fetching endHandle. */
		//pc.printf("progressService4 \r\n");  
        if (endHandle == SRV_DISC_END_HANDLE) {
			//pc.printf("terminateServiceDiscovery \r\n"); 
			if(  characteristicCallback )
			{	/* need to scan characteristics */
				if(matchServiceIndex != 0xFF && matchCharsIndex == 0xFF){
					//pc.printf("start Scan chars \r\n");  
					//pc.printf("startHandle : %2x \r\n", services[matchServiceIndex].getStartHandle());
					//pc.printf("endHandle : %2x \r\n", services[matchServiceIndex].getEndHandle());
					launchCharacteristicDiscovery(connHandle, services[matchServiceIndex].getStartHandle(), services[matchServiceIndex].getEndHandle());		
				}
				else if(matchServiceIndex != 0xFF && matchCharsIndex != 0xFF){
					/* after scan characteristic and descriptors */
					characteristicCallback(&characteristics[matchCharsIndex]);
					terminateServiceDiscovery();
				}
				else {
					terminateServiceDiscovery();
				}
			}
			else {
				terminateServiceDiscovery();
			}
        } 
		else {
			/* continue scan services */
            if (sd_ble_gattc_primary_services_discover(connHandle, endHandle, NULL) != NRF_SUCCESS) { 
                terminateServiceDiscovery();
            }
        }
    }
	//pc.printf("progressServiceDiscovery end//\r\n");  
}

void 
nRF51ServiceDiscovery::progressDescriptorDiscovery(void)
{
	if( state == DESCRIPTOR_DISCOVERY_START )
	{	//pc.printf("progressDescriptorDiscovery end//\r\n");  
		state = DESCRIPTOR_DISCOVERY_ACTIVE;
		if( matchServiceIndex != 0xFF && matchCharsIndex != 0xFF ){
			
			ble_gattc_handle_range_t handleRange = {
				.start_handle = characteristics[matchCharsIndex].getValueHandle() + 1,
				.end_handle   = services[matchServiceIndex].getEndHandle()
			};
			//pc.printf("startHandle : %2x \r\n", handleRange.start_handle);
			//pc.printf("endHandle : %2x \r\n", handleRange.end_handle);
			if(handleRange.start_handle < handleRange.end_handle) {
				uint32_t err_code = sd_ble_gattc_descriptors_discover(connHandle, &handleRange);
				if(err_code != NRF_SUCCESS){
					//pc.printf("err_code : %2x \r\n", err_code);
					terminateDescriptorDiscovery();
				}
			}
			else {
				terminateDescriptorDiscovery();
			}
		}
		else {
			terminateDescriptorDiscovery();
		}	
	}
}

void
nRF51ServiceDiscovery::ServiceUUIDDiscoveryQueue::triggerFirst(void)
{
	//pc.printf("ServiceUUIDDiscoveryQueue::triggerFirst \r\n");  
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
		//pc.printf("ServiceUUIDDiscoveryQueue 1 \r\n");  
        if (sd_ble_gattc_char_value_by_uuid_read(parentDiscoveryObject->connHandle, &uuid, &handleRange) == NRF_SUCCESS) {
            return;
        }

        /* Skip this service if we fail to launch a read for its service-declaration
         * attribute. Its UUID will remain INVALID, and it may not match any filters. */
        dequeue();
    }

    /* Switch back to service discovery upon exhausting the service-indices pending UUID discovery. */
    if (parentDiscoveryObject->state == DISCOVER_SERVICE_UUIDS) {
		//pc.printf("ServiceUUIDDiscoveryQueue 2 \r\n");  
        parentDiscoveryObject->state = SERVICE_DISCOVERY_ACTIVE;
    }
	//pc.printf("ServiceUUIDDiscoveryQueue::triggerFirst end//\r\n");
}

void
nRF51ServiceDiscovery::CharUUIDDiscoveryQueue::triggerFirst(void)
{
	//pc.printf("CharUUIDDiscoveryQueue::triggerFirst \r\n");  
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
		//pc.printf("CharUUIDDiscoveryQueue 1 \r\n");  
        if (sd_ble_gattc_char_value_by_uuid_read(parentDiscoveryObject->connHandle, &uuid, &handleRange) == NRF_SUCCESS) {
            return;
        }

        /* Skip this service if we fail to launch a read for its service-declaration
         * attribute. Its UUID will remain INVALID, and it may not match any filters. */
        dequeue();
    }

    /* Switch back to service discovery upon exhausting the service-indices pending UUID discovery. */
    if (parentDiscoveryObject->state == DISCOVER_CHARACTERISTIC_UUIDS) {
		//pc.printf("CharUUIDDiscoveryQueue 2 \r\n");  
        parentDiscoveryObject->state = CHARACTERISTIC_DISCOVERY_ACTIVE;
    }
	//pc.printf("CharUUIDDiscoveryQueue::triggerFirst end//\r\n");
}

void
nRF51ServiceDiscovery::processDiscoverUUIDResponse(const ble_gattc_evt_char_val_by_uuid_read_rsp_t *response)
{
	//pc.printf("processDiscoverUUIDResponse \r\n");  
    if (state == DISCOVER_SERVICE_UUIDS) {
		//pc.printf("DISCOVER_SERVICE_UUIDS \r\n");  
        if ((response->count == 1) && (response->value_len == UUID::LENGTH_OF_LONG_UUID)) {
            UUID::LongUUIDBytes_t uuid;
            /* Switch longUUID bytes to MSB */
			//pc.printf("DISCOVER_SERVICE_UUIDS is : "); 
            for (unsigned i = 0; i < UUID::LENGTH_OF_LONG_UUID; i++) {
                uuid[i] = response->handle_value[0].p_value[UUID::LENGTH_OF_LONG_UUID - 1 - i];
				//pc.printf("%x ", uuid[i]); 
            }
			//pc.printf("\r\n"); 
			//pc.printf("DISCOVER_SERVICE_UUIDS 1 \r\n");  
            unsigned serviceIndex = serviceUUIDDiscoveryQueue.dequeue();
            services[serviceIndex].setupLongUUID(uuid);

            serviceUUIDDiscoveryQueue.triggerFirst();
        } else {
			//pc.printf("DISCOVER_SERVICE_UUIDS 2 \r\n");  
            serviceUUIDDiscoveryQueue.dequeue();
        }
    } else if (state == DISCOVER_CHARACTERISTIC_UUIDS) {
		//pc.printf("DISCOVER_CHARACTERISTIC_UUIDS \r\n");  
        if ((response->count == 1) && (response->value_len == UUID::LENGTH_OF_LONG_UUID + 1 /* props */ + 2 /* value handle */)) {
            UUID::LongUUIDBytes_t uuid;
            /* Switch longUUID bytes to MSB */
			//pc.printf("DISCOVER_CHARACTERISTIC_UUIDS is : "); 
            for (unsigned i = 0; i < UUID::LENGTH_OF_LONG_UUID; i++) {
                uuid[i] = response->handle_value[0].p_value[3 + UUID::LENGTH_OF_LONG_UUID - 1 - i];
				//pc.printf("%x ", uuid[i]); 
            }
			//pc.printf("\r\n");
			//pc.printf("DISCOVER_CHARACTERISTIC_UUIDS 1\r\n");  
            unsigned charIndex = charUUIDDiscoveryQueue.dequeue();
            characteristics[charIndex].setupLongUUID(uuid);

            charUUIDDiscoveryQueue.triggerFirst();
        } else {
			//pc.printf("DISCOVER_CHARACTERISTIC_UUIDS 2\r\n");  
            charUUIDDiscoveryQueue.dequeue();
        }
    }
	//pc.printf("processDiscoverUUIDResponse end//\r\n");
}
