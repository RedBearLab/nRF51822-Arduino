/*
 * Copyright (c) 2016 RedBear
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include <BLE_API.h>

#define DEVICE_NAME       "BLE_Peripheral"
#define TXRX_BUF_LEN      20
// Create ble instance
BLE                       ble;
// Create a timer task
Ticker                    ticker1s;

// The uuid of service and characteristics
static const uint8_t service1_uuid[]         = {0x71, 0x3D, 0, 0, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t service1_chars1_uuid[]  = {0x71, 0x3D, 0, 2, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t service1_chars2_uuid[]  = {0x71, 0x3D, 0, 3, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t service1_chars3_uuid[]  = {0x71, 0x3D, 0, 4, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
// Used in advertisement
static const uint8_t uart_base_uuid_rev[]    = {0x1E, 0x94, 0x8D, 0xF1, 0x48, 0x31, 0x94, 0xBA, 0x75, 0x4C, 0x3E, 0x50, 0, 0, 0x3D, 0x71};

// Initialize value of chars
uint8_t chars1_value[TXRX_BUF_LEN] = {0};
uint8_t chars2_value[TXRX_BUF_LEN] = {1,2,3};
uint8_t chars3_value[TXRX_BUF_LEN] = {0};

// Create characteristic
GattCharacteristic  characteristic1(service1_chars1_uuid, chars1_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE );
GattCharacteristic  characteristic2(service1_chars2_uuid, chars2_value, 3, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ);
GattCharacteristic  characteristic3(service1_chars3_uuid, chars3_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);
GattCharacteristic *uartChars[] = {&characteristic1, &characteristic2, &characteristic3};
//Create service
GattService         uartService(service1_uuid, uartChars, sizeof(uartChars) / sizeof(GattCharacteristic *));

DFUService               *dfuService;
HeartRateService         *hrService;
DeviceInformationService *deviceInfo;
// Init HRM to 100bps
static uint16_t hrmCounter = 100;
/** @brief  Disconnect callback handle
 *
 *  @param[in] *params   params->handle : connect handle
 *                       params->reason : CONNECTION_TIMEOUT                          = 0x08,
 *                                        REMOTE_USER_TERMINATED_CONNECTION           = 0x13,
 *                                        REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES = 0x14,  // Remote device terminated connection due to low resources.
 *                                        REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF     = 0x15,  // Remote device terminated connection due to power off.
 *                                        LOCAL_HOST_TERMINATED_CONNECTION            = 0x16,
 *                                        CONN_INTERVAL_UNACCEPTABLE                  = 0x3B,
 */
void disconnectionCallBack(const Gap::DisconnectionCallbackParams_t *params) {
  Serial.print("Disconnected hande : ");
  Serial.println(params->handle, HEX);
  Serial.print("Disconnected reason : ");
  Serial.println(params->reason, HEX);
  Serial.println("Restart advertising ");
  ble.startAdvertising();
}

/** @brief  Connection callback handle
 *
 *  @param[in] *params   params->handle : The ID for this connection
 *                       params->role : PERIPHERAL  = 0x1, // Peripheral Role
 *                                      CENTRAL     = 0x2, // Central Role.
 *                       params->peerAddrType : The peer's BLE address type
 *                       params->peerAddr : The peer's BLE address
 *                       params->ownAddrType : This device's BLE address type
 *                       params->ownAddr : This devices's BLE address
 *                       params->connectionParams->minConnectionInterval
 *                       params->connectionParams->maxConnectionInterval
 *                       params->connectionParams->slaveLatency
 *                       params->connectionParams->connectionSupervisionTimeout
 */
void connectionCallBack( const Gap::ConnectionCallbackParams_t *params ) {
  uint8_t index;
  if(params->role == Gap::PERIPHERAL) {
    Serial.println("Peripheral ");
  }

  Serial.print("The conn handle : ");
  Serial.println(params->handle, HEX);
  Serial.print("The conn role : ");
  Serial.println(params->role, HEX);

  Serial.print("The peerAddr type : ");
  Serial.println(params->peerAddrType, HEX);
  Serial.print("  The peerAddr : ");
  for(index=0; index<6; index++) {
    Serial.print(params->peerAddr[index], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");

  Serial.print("The ownAddr type : ");
  Serial.println(params->ownAddrType, HEX);
  Serial.print("  The ownAddr : ");
  for(index=0; index<6; index++) {
    Serial.print(params->ownAddr[index], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");

  Serial.print("The min connection interval : ");
  Serial.println(params->connectionParams->minConnectionInterval, HEX);
  Serial.print("The max connection interval : ");
  Serial.println(params->connectionParams->maxConnectionInterval, HEX);
  Serial.print("The slaveLatency : ");
  Serial.println(params->connectionParams->slaveLatency, HEX);
  Serial.print("The connectionSupervisionTimeout : ");
  Serial.println(params->connectionParams->connectionSupervisionTimeout, HEX);
}

/** @brief  write callback handle of Gatt server
 *
 *  @param[in] *Handler   Handler->connHandle : The handle of the connection that triggered the event
 *                        Handler->handle : Attribute Handle to which the write operation applies
 *                        Handler->writeOp : OP_INVALID               = 0x00,  // Invalid operation.
 *                                           OP_WRITE_REQ             = 0x01,  // Write request.
 *                                           OP_WRITE_CMD             = 0x02,  // Write command.
 *                                           OP_SIGN_WRITE_CMD        = 0x03,  // Signed write command.
 *                                           OP_PREP_WRITE_REQ        = 0x04,  // Prepare write request.
 *                                           OP_EXEC_WRITE_REQ_CANCEL = 0x05,  // Execute write request: cancel all prepared writes.
 *                                           OP_EXEC_WRITE_REQ_NOW    = 0x06,  // Execute write request: immediately execute all prepared writes.
 *                        Handler->offset : Offset for the write operation
 *                        Handler->len : Length (in bytes) of the data to write
 *                        Handler->data : Pointer to the data to write
 */
void gattServerWriteCallBack(const GattWriteCallbackParams *Handler) {
  uint8_t index;

  Serial.print("Handler->connHandle : ");
  Serial.println(Handler->connHandle, HEX);
  Serial.print("Handler->handle : ");
  Serial.println(Handler->handle, HEX);
  Serial.print("Handler->writeOp : ");
  Serial.println(Handler->writeOp, HEX);
  Serial.print("Handler->offset : ");
  Serial.println(Handler->offset, HEX);
  Serial.print("Handler->len : ");
  Serial.println(Handler->len, HEX);
  for(index=0; index<Handler->len; index++) {
    Serial.print(Handler->data[index], HEX);
  }
  Serial.println(" ");

  uint8_t buf[TXRX_BUF_LEN];
  uint16_t bytesRead;

  Serial.println("Write Handle : ");
  // Check the attribute belong to which characteristic
  if (Handler->handle == characteristic1.getValueAttribute().getHandle()) {
    // Read the value of characteristic
    ble.readCharacteristicValue(characteristic1.getValueAttribute().getHandle(), buf, &bytesRead);
    for(index=0; index<bytesRead; index++) {
      Serial.print(buf[index], HEX);
    }
    Serial.println(" ");
  }
}

/** @brief  Helper function that notifies all registered handlers of an occurrence of a data sent event. 
 *          This function is meant to be called from the BLE stack specific implementation when a data sent event occurs(event: BLE_EVT_TX_COMPLETE).
 *
 *  @param[in] count   Number of packets sent.
 */
void gattServerSentCallBack(unsigned int count)
{
  Serial.print("The count : ");
  Serial.println(count, DEC);
}

/**
 * @brief  Timer task callback handle
 */
void task_handle(void) {
  static uint16_t value;
  Serial.println("Task handle ");
  value++;
  // if true, saved to attribute, no notification or indication is generated.
  //ble.updateCharacteristicValue(characteristic3.getValueAttribute().getHandle(), (uint8_t *)&value, 2, true);
  // if false or ignore, notification or indication is generated if permit.
  ble.updateCharacteristicValue(characteristic3.getValueAttribute().getHandle(), (uint8_t *)&value, 2);
  // update heart rate
  hrmCounter++;
  if (hrmCounter == 175) { //  100 <= HRM bps <=175
      hrmCounter = 100;
  }
  hrService->updateHeartRate(hrmCounter);
}

/**
 * @brief  Set advertisement
 */
void setAdvertisement(void) {
  // A list of Advertising Data types commonly used by peripherals.
  //   FLAGS                              = 0x01, // Flags, refer to GapAdvertisingData::Flags_t.
  //   INCOMPLETE_LIST_16BIT_SERVICE_IDS  = 0x02, // Incomplete list of 16-bit Service IDs.
  //   COMPLETE_LIST_16BIT_SERVICE_IDS    = 0x03, // Complete list of 16-bit Service IDs.
  //   INCOMPLETE_LIST_32BIT_SERVICE_IDS  = 0x04, // Incomplete list of 32-bit Service IDs (not relevant for Bluetooth 4.0).
  //   COMPLETE_LIST_32BIT_SERVICE_IDS    = 0x05, // Complete list of 32-bit Service IDs (not relevant for Bluetooth 4.0).
  //   INCOMPLETE_LIST_128BIT_SERVICE_IDS = 0x06, // Incomplete list of 128-bit Service IDs.
  //   COMPLETE_LIST_128BIT_SERVICE_IDS   = 0x07, // Complete list of 128-bit Service IDs.
  //   SHORTENED_LOCAL_NAME               = 0x08, // Shortened Local Name.
  //   COMPLETE_LOCAL_NAME                = 0x09, // Complete Local Name.
  //   TX_POWER_LEVEL                     = 0x0A, // TX Power Level (in dBm).
  //   DEVICE_ID                          = 0x10, // Device ID.
  //   SLAVE_CONNECTION_INTERVAL_RANGE    = 0x12, // Slave Connection Interval Range.
  //   LIST_128BIT_SOLICITATION_IDS       = 0x15, // List of 128 bit service UUIDs the device is looking for.
  //   SERVICE_DATA                       = 0x16, // Service Data.
  //   APPEARANCE                         = 0x19, // Appearance, refer to GapAdvertisingData::Appearance_t.
  //   ADVERTISING_INTERVAL               = 0x1A, // Advertising Interval.
  //   MANUFACTURER_SPECIFIC_DATA         = 0xFF  // Manufacturer Specific Data.

  // AD_Type_Flag : LE_LIMITED_DISCOVERABLE = 0x01, Peripheral device is discoverable for a limited period of time
  //                LE_GENERAL_DISCOVERABLE = 0x02, Peripheral device is discoverable at any moment
  //                BREDR_NOT_SUPPORTED     = 0x03, Peripheral device is LE only
  //                SIMULTANEOUS_LE_BREDR_C = 0x04, Not relevant - central mode only
  //                SIMULTANEOUS_LE_BREDR_H = 0x05, Not relevant - central mode only
  ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
  // Add short name to advertisement
  ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,(const uint8_t *)"TXRX", 4);
  // Add complete 128bit_uuid to advertisement
  ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,(const uint8_t *)uart_base_uuid_rev, sizeof(uart_base_uuid_rev));
  // Add complete device name to scan response data
  ble.accumulateScanResponse(GapAdvertisingData::COMPLETE_LOCAL_NAME,(const uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME) - 1);
}

void setup() {
  // put your setup code here, to run once
  Serial.begin(9600);
  Serial.println("Start ");
  pinMode(D13, OUTPUT);
  // Init timer task
  ticker1s.attach(task_handle, 1);
  // Init ble
  ble.init();
  ble.onConnection(connectionCallBack);
  ble.onDisconnection(disconnectionCallBack);
  ble.onDataWritten(gattServerWriteCallBack);
  ble.onDataSent(gattServerSentCallBack);
  // set advertisement
  setAdvertisement();
  // set adv_type(enum from 0)
  //    ADV_CONNECTABLE_UNDIRECTED
  //    ADV_CONNECTABLE_DIRECTED
  //    ADV_SCANNABLE_UNDIRECTED
  //    ADV_NON_CONNECTABLE_UNDIRECTED
  ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
  // add service
  dfuService = new DFUService(ble);
  hrService  = new HeartRateService(ble, hrmCounter, HeartRateService::LOCATION_FINGER);
  deviceInfo = new DeviceInformationService(ble, "ARM", "Model1", "SN1", "hw-rev1", "fw-rev1", "soft-rev1");
  ble.addService(uartService);
  // set device name
  ble.setDeviceName((const uint8_t *)DEVICE_NAME);
  // set tx power,valid values are -40, -20, -16, -12, -8, -4, 0, 4
  ble.setTxPower(4);
  // set adv_interval, 100ms in multiples of 0.625ms.
  ble.setAdvertisingInterval(160);
  // set adv_timeout, in seconds
  ble.setAdvertisingTimeout(0);
  // ger BLE stack version
  Serial.print("BLE stack verison is : ");
  Serial.println(ble.getVersion());
  // start advertising
  ble.startAdvertising();
  Serial.println("start advertising ");
}

void loop() {
  // put your main code here, to run repeatedly:
  ble.waitForEvent();
}

