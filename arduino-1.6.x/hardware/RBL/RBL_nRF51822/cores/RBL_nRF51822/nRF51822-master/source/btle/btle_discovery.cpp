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
#include "nRF51GattClient.h"

//extern Serial pc;

void bleGattcEventHandler(const ble_evt_t *p_ble_evt)
{
    nRF51ServiceDiscovery &sdSingleton = nRF51GattClient::getInstance().discovery;

    switch (p_ble_evt->header.evt_id) {
        case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
			//pc.printf("EVT : BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP \r\n");  
            switch (p_ble_evt->evt.gattc_evt.gatt_status) {
                case BLE_GATT_STATUS_SUCCESS:
                    sdSingleton.setupDiscoveredServices(&p_ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp);
                    break;

                case BLE_GATT_STATUS_ATTERR_ATTRIBUTE_NOT_FOUND:
                default:
                    sdSingleton.terminate();
                    break;
            }
            break;

        case BLE_GATTC_EVT_CHAR_DISC_RSP:
			//pc.printf("EVT : BLE_GATTC_EVT_CHAR_DISC_RSP \r\n");  
            switch (p_ble_evt->evt.gattc_evt.gatt_status) {
                case BLE_GATT_STATUS_SUCCESS:
                    sdSingleton.setupDiscoveredCharacteristics(&p_ble_evt->evt.gattc_evt.params.char_disc_rsp);
                    break;

                case BLE_GATT_STATUS_ATTERR_ATTRIBUTE_NOT_FOUND:
				//pc.printf("EVT : BLE_GATT_STATUS_ATTERR_ATTRIBUTE_NOT_FOUND \r\n"); 
                default:
                    sdSingleton.terminateCharacteristicDiscovery();
                    break;
            }
            break;

        case BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP:
			//pc.printf("EVT : BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP \r\n");  
            if (sdSingleton.isActive()) {
                sdSingleton.processDiscoverUUIDResponse(&p_ble_evt->evt.gattc_evt.params.char_val_by_uuid_read_rsp);
            }
            break;
			
		case BLE_GATTC_EVT_DESC_DISC_RSP:
			//pc.printf("EVT : BLE_GATTC_EVT_DESC_DISC_RSP \r\n");  
			switch(p_ble_evt->evt.gattc_evt.gatt_status){
				case BLE_GATT_STATUS_SUCCESS : 
					sdSingleton.setupDiscoveredDescriptor(&p_ble_evt->evt.gattc_evt.params.desc_disc_rsp);
					break;
				case BLE_GATT_STATUS_ATTERR_ATTRIBUTE_NOT_FOUND:
					//pc.printf("EVT : BLE_GATT_STATUS_ATTERR_ATTRIBUTE_NOT_FOUND \r\n"); 
                default:
                    sdSingleton.terminateDescriptorDiscovery();
                    break;
			}
			break;

        case BLE_GATTC_EVT_READ_RSP: {
			//pc.printf("EVT : BLE_GATTC_EVT_READ_RSP \r\n"); 
                GattReadCallbackParams response = {
                    .handle = p_ble_evt->evt.gattc_evt.params.read_rsp.handle,
                    .offset = p_ble_evt->evt.gattc_evt.params.read_rsp.offset,
                    .len    = p_ble_evt->evt.gattc_evt.params.read_rsp.len,
                    .data   = p_ble_evt->evt.gattc_evt.params.read_rsp.data,
                };
                nRF51GattClient::getInstance().processReadResponse(&response);
            }
            break;

        case BLE_GATTC_EVT_WRITE_RSP: {
			//pc.printf("EVT : BLE_GATTC_EVT_WRITE_RSP \r\n"); 
                GattWriteCallbackParams response = {
                    .handle  = p_ble_evt->evt.gattc_evt.params.write_rsp.handle,
                    .writeOp = (GattWriteCallbackParams::WriteOp_t)(p_ble_evt->evt.gattc_evt.params.write_rsp.write_op),
                    .offset  = p_ble_evt->evt.gattc_evt.params.write_rsp.offset,
                    .len     = p_ble_evt->evt.gattc_evt.params.write_rsp.len,
                    .data    = p_ble_evt->evt.gattc_evt.params.write_rsp.data,
                };
                nRF51GattClient::getInstance().processWriteResponse(&response);
            }
            break;

        case BLE_GATTC_EVT_HVX: {
			//pc.printf("EVT : BLE_GATTC_EVT_HVX \r\n"); 
                GattHVXCallbackParams params;
                params.handle = p_ble_evt->evt.gattc_evt.params.hvx.handle;
                params.type   = static_cast<HVXType_t>(p_ble_evt->evt.gattc_evt.params.hvx.type);
                params.len    = p_ble_evt->evt.gattc_evt.params.hvx.len;
                params.data   = p_ble_evt->evt.gattc_evt.params.hvx.data;

                nRF51GattClient::getInstance().processHVXEvent(&params);
            }
            break;
    }

    sdSingleton.progressCharacteristicDiscovery();
	sdSingleton.progressDescriptorDiscovery();
    sdSingleton.progressServiceDiscovery();
}

