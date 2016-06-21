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
/**
 * @brief This example is used to demonstrate central functions.
 *        Device with this application will try to connect to example "BLE_HRM" or "BLE_simplePeripheral"
 *        If connecting to "BLE_HRM", will get heart rate
 *        If connecting to "BLE_simplePeripheral, will discovery all services and characteristics
 */
#include <BLE_API.h>

BLE           ble;

static uint8_t service1_uuid[]    = {0x71, 0x3D, 0, 0, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static uint8_t service1_chars1[]  = {0x71, 0x3D, 0, 2, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static uint8_t service1_chars2[]  = {0x71, 0x3D, 0, 3, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static uint8_t service1_chars3[]  = {0x71, 0x3D, 0, 4, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};

UUID service_uuid(0x180D);
UUID chars_uuid1(0x2A37);
UUID chars_uuid2(service1_chars2);
UUID chars_uuid3(service1_chars3);

static uint8_t device_is_hrm = 0;
static uint8_t device_is_simple_peripheral = 0;

// When found the match characteristic, set 1.
static uint8_t characteristic_is_fond = 0;
// When found the match descriptor, set 1.
static uint8_t descriptor_is_found = 0;
// To save the hrm characteristic and descriptor
static DiscoveredCharacteristic            chars_hrm;
static DiscoveredCharacteristicDescriptor  desc_of_chars_hrm(NULL,GattAttribute::INVALID_HANDLE,GattAttribute::INVALID_HANDLE,UUID::ShortUUIDBytes_t(0));

static void scanCallBack(const Gap::AdvertisementCallbackParams_t *params);
static void discoveredServiceCallBack(const DiscoveredService *service);
static void discoveredCharacteristicCallBack(const DiscoveredCharacteristic *chars);
static void discoveryTerminationCallBack(Gap::Handle_t connectionHandle);
static void discoveredCharsDescriptorCallBack(const CharacteristicDescriptorDiscovery::DiscoveryCallbackParams_t *params);
static void discoveredDescTerminationCallBack(const CharacteristicDescriptorDiscovery::TerminationCallbackParams_t *params) ;

/**
 * @brief  Function to decode advertisement or scan response data
 *
 * @param[in]  type            The data type that you want to get
 * @param[in]  advdata_len     The length of advertisement or scan reponse data
 * @param[in]  *p_advdata      The pointer of advertisement or scan reponse data
 * @param[out] *len            If type exist, this is the length of field data
 * @param[out] *p_field_data   If type exist, this is the pointer of field data
 *
 * @return NRF_SUCCESS or NRF_ERROR_NOT_FOUND
 */
uint32_t ble_advdata_parser(uint8_t type, uint8_t advdata_len, uint8_t *p_advdata, uint8_t *len, uint8_t *p_field_data) {
  uint8_t index=0;
  uint8_t field_length, field_type;

  while(index<advdata_len) {
    field_length = p_advdata[index];
    field_type   = p_advdata[index+1];
    if(field_type == type) {
      memcpy(p_field_data, &p_advdata[index+2], (field_length-1));
      *len = field_length - 1;
      return NRF_SUCCESS;
    }
    index += field_length + 1;
  }
  return NRF_ERROR_NOT_FOUND;
}

void startDiscovery(uint16_t handle) {
  /**
   * Launch service discovery. Once launched, application callbacks will beinvoked for matching services or characteristics.
   * isServiceDiscoveryActive() can be used to determine status, and a termination callback (if one was set up)will be invoked at the end.
   * Service discovery can be terminated prematurely,if needed, using terminateServiceDiscovery().
   *
   * @param[in]  connectionHandle   Handle for the connection with the peer.
   * @param[in]  sc  This is the application callback for a matching service. Taken as NULL by default.
   *                 Note: service discovery may still be active when this callback is issued;
   *                 calling asynchronous BLE-stack APIs from within this application callback might cause the stack to abort service discovery.
   *                 If this becomes an issue, it may be better to make a local copy of the discoveredService and wait for service discovery to terminate before operating on the service.
   * @param[in]  cc  This is the application callback for a matching characteristic.Taken as NULL by default.
   *                 Note: service discovery may still be active when this callback is issued;
   *                 calling asynchronous BLE-stack APIs from within this application callback might cause the stack to abort service discovery.
   *                 If this becomes an issue, it may be better to make a local copy of the discoveredCharacteristic and wait for service discovery to terminate before operating on the characteristic.
   * @param[in]  matchingServiceUUID  UUID-based filter for specifying a service in which the application is interested.
   *                                  By default it is set as the wildcard UUID_UNKNOWN, in which case it matches all services.
   * @param[in]  matchingCharacteristicUUIDIn  UUID-based filter for specifying characteristic in which the application is interested.
   *                                           By default it is set as the wildcard UUID_UKNOWN to match against any characteristic.
   *
   * @note     Using wildcard values for both service-UUID and characteristic-UUID will result in complete service discovery:
   *           callbacks being called for every service and characteristic.
   *
   * @note     Providing NULL for the characteristic callback will result in characteristic discovery being skipped for each matching service.
   *           This allows for an inexpensive method to discover only services.
   *
   * @return   BLE_ERROR_NONE if service discovery is launched successfully; else an appropriate error.
   */
  if(device_is_hrm)
    ble.gattClient().launchServiceDiscovery(handle, discoveredServiceCallBack, discoveredCharacteristicCallBack, service_uuid, chars_uuid1);
  if(device_is_simple_peripheral)
    ble.gattClient().launchServiceDiscovery(handle, discoveredServiceCallBack, discoveredCharacteristicCallBack);
}

/**
 * @brief  Callback handle for scanning device
 *
 * @param[in]  *params   params->peerAddr            The peer's BLE address
 *                       params->rssi                The advertisement packet RSSI value
 *                       params->isScanResponse      Whether this packet is the response to a scan request
 *                       params->type                The type of advertisement
 *                                                   (enum from 0 ADV_CONNECTABLE_UNDIRECTED,ADV_CONNECTABLE_DIRECTED,ADV_SCANNABLE_UNDIRECTED,ADV_NON_CONNECTABLE_UNDIRECTED)
 *                       params->advertisingDataLen  Length of the advertisement data
 *                       params->advertisingData     Pointer to the advertisement packet's data
 */
static void scanCallBack(const Gap::AdvertisementCallbackParams_t *params) {
  uint8_t index;

  Serial.println("Scan CallBack ");
  Serial.print("PerrAddress: ");
  for(index=0; index<6; index++) {
    Serial.print(params->peerAddr[index], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");

  Serial.print("The Rssi : ");
  Serial.println(params->rssi, DEC);

  Serial.print("The adv_data : ");
  Serial.println((const char*)params->advertisingData);

  uint8_t len;
  uint8_t adv_name[31];
  if( NRF_SUCCESS == ble_advdata_parser(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, params->advertisingDataLen, (uint8_t *)params->advertisingData, &len, adv_name) ) {
    Serial.print("Short name len : ");
    Serial.println(len, DEC);
    Serial.print("Short name is : ");
    Serial.println((const char*)adv_name);
    if( memcmp("TXRX", adv_name, 4) == 0x00 ) {
      Serial.println("Got device, stop scan ");
      ble.stopScan();
      /**
      * @param[in]  peerAddrType    BLEProtocol::AddressType::PUBLIC
      *                             BLEProtocol::AddressType::RANDOM_STATIC
      *                             BLEProtocol::AddressType::RANDOM_PRIVATE_RESOLVABLE
      *                             BLEProtocol::AddressType::RANDOM_PRIVATE_NON_RESOLVABLE
      * @param[in]  *connectionParams
      * @param[in]  *scanParams
      *
      * @return BLE_ERROR_NONE if connection establishment procedure is started successfully. The connectionCallChain (if set) will be invoked upon a connection event.
      */
      device_is_simple_peripheral = 1;
      ble.connect(params->peerAddr, BLEProtocol::AddressType::RANDOM_STATIC, NULL, NULL);
    }
  }
  else if( NRF_SUCCESS == ble_advdata_parser(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, params->advertisingDataLen, (uint8_t *)params->advertisingData, &len, adv_name) ) {
    Serial.print("Complete name len : ");
    Serial.println(len, DEC);
    Serial.print("Complete name is : ");
    Serial.println((const char*)adv_name);

    if(memcmp("Nordic_HRM", adv_name, 10) == 0x00) {
      Serial.println("Got device, stop scan ");
      ble.stopScan();
      device_is_hrm = 1;
      ble.connect(params->peerAddr, BLEProtocol::AddressType::RANDOM_STATIC, NULL, NULL);
    }
  }
  Serial.println(" ");
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

  Serial.print("The conn handle : ");
  Serial.println(params->handle, HEX);

  Serial.print("  The peerAddr : ");
  for(index=0; index<6; index++) {
    Serial.print(params->peerAddr[index], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");
  // start to discovery
  startDiscovery(params->handle);
}

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
  Serial.println("Disconnected, start to scanning");
  device_is_simple_peripheral = 0;
  device_is_hrm = 0;
  characteristic_is_fond = 0;
  descriptor_is_found = 0;
  ble.startScan(scanCallBack);
}

/** @brief Discovered service callback handle
 *
 *  @param[in] *service  service->getUUID()  The UUID of service
 *                       service->getStartHandle()
 *                       service->getEndHandle()
 */
static void discoveredServiceCallBack(const DiscoveredService *service) {
  Serial.println("\r\n----Servuce Discovered");

  Serial.print("Service UUID type        : ");
  Serial.println(service->getUUID().shortOrLong(), HEX);// 0 16bit_uuid, 1 128bit_uuid
  Serial.print("Service UUID             : ");
  if(service->getUUID().shortOrLong() == UUID::UUID_TYPE_SHORT) {
    Serial.println(service->getUUID().getShortUUID(), HEX);
  }
  else {
    uint8_t index;
    const uint8_t *uuid = service->getUUID().getBaseUUID();
    for(index=0; index<16; index++) {
      Serial.print(uuid[index], HEX);
      Serial.print(" ");
    }
    Serial.println(" ");
  }
  Serial.print("The service start handle : ");
  Serial.println(service->getStartHandle(), HEX);
  Serial.print("The service end handle   : ");
  Serial.println(service->getEndHandle(), HEX);
}

/** @brief Discovered characteristics callback handle
 *
 *  @param[in] *chars  chars->getUUID()        The UUID of service
 *                     chars->getProperties()  broadcast() : Check if broadcasting is permitted. True is permited.
 *                                             read() : Check reading is permitted.
 *                                             writeWoResp() : Check if writing with Write Command is permitted
 *                                             write() : Check if writing with Write Request is permitted.
 *                                             notify() : Check notifications are permitted.
 *                                             indicate() : Check if indications are permitted.
 *                                             authSignedWrite() : Check if writing with Signed Write Command is permitted.
 *                     chars->getDeclHandle()  characteristic's declaration attribute handle
 *                     chars->getValueHandle() characteristic's value attribute handle
 *                     chars->getLastHandle()  characteristic's last attribute handle
 */
static void discoveredCharacteristicCallBack(const DiscoveredCharacteristic *chars) {
  Serial.println("\r\n----Characteristic Discovered");
  Serial.print("Chars UUID type        : ");
  Serial.println(chars->getUUID().shortOrLong(), HEX);// 0 16bit_uuid, 1 128bit_uuid
  Serial.print("Chars UUID             : ");
  if(chars->getUUID().shortOrLong() == UUID::UUID_TYPE_SHORT) {
    Serial.println(chars->getUUID().getShortUUID(), HEX);
    if(chars->getUUID().getShortUUID() == 0x2A37) {
      Serial.println("Found HRM characteristic ");
      characteristic_is_fond = 1;
      chars_hrm = *chars;
    }
  }
  else {
    uint8_t index;
    const uint8_t *uuid = chars->getUUID().getBaseUUID();
    for(index=0; index<16; index++) {
      Serial.print(uuid[index], HEX);
      Serial.print(" ");
    }
    Serial.println(" ");
  }

  Serial.print("properties_read        : ");
  Serial.println(chars->getProperties().read(), DEC);
  Serial.print("properties_writeWoResp : ");
  Serial.println(chars->getProperties().writeWoResp(), DEC);
  Serial.print("properties_write       : ");
  Serial.println(chars->getProperties().write(), DEC);
  Serial.print("properties_notify      : ");
  Serial.println(chars->getProperties().notify(), DEC);

  Serial.print("declHandle             : ");
  Serial.println(chars->getDeclHandle(), HEX);
  Serial.print("valueHandle            : ");
  Serial.println(chars->getValueHandle(), HEX);
  Serial.print("lastHandle             : ");
  Serial.println(chars->getLastHandle(), HEX);

  //uint16_t value = 0x0001;
  //ble.gattClient().read(chars->getConnHandle(), chars->getValueHandle(), 0);
  //ble.gattClient().write(GattClient::GATT_OP_WRITE_CMD,chars->getConnHandle(),chars->getValueHandle(),2,(uint8_t *)&value);
}

/**
 * @brief Discovered service and characteristics termination callback handle
 */
static void discoveryTerminationCallBack(Gap::Handle_t connectionHandle) {
  Serial.println("\r\n----discoveryTermination");
  Serial.print("connectionHandle       : ");
  Serial.println(connectionHandle, HEX);
  if(characteristic_is_fond == 1) {
    ble.gattClient().discoverCharacteristicDescriptors(chars_hrm, discoveredCharsDescriptorCallBack, discoveredDescTerminationCallBack);
  }
}

/** @brief Discovered descriptor of characteristic callback handle
 *
 *  @param[in] *params  params->characteristic  DiscoveredCharacteristic
 *                      params->descriptor  descriptor.getUUID() : The uuid of descriptor atrribute
 *                                          descriptor.getConnectionHandle()
 *                                          descriptor.getAttributeHandle() : Arrtibute handle
 */
static void discoveredCharsDescriptorCallBack(const CharacteristicDescriptorDiscovery::DiscoveryCallbackParams_t *params) {
  Serial.println("\r\n----discovered descriptor");
  Serial.print("Desriptor UUID         : ");
  Serial.println(params->descriptor.getUUID().getShortUUID(), HEX);
  if(params->descriptor.getUUID().getShortUUID() == 0x2902) {
    // Save characteristic info
    descriptor_is_found = 1;
    desc_of_chars_hrm = params->descriptor;
  }
  Serial.print("connectionHandle       : ");
  Serial.println(params->descriptor.getConnectionHandle(), HEX);
  Serial.print("descriptor Handle      : ");
  Serial.println(params->descriptor.getAttributeHandle(), HEX);
}

/** @brief Discovered descriptor of characteristic termination callback handle
 *
 *  @param[in] *params  params->characteristic  DiscoveredCharacteristic
 *                      params->status  Status of the discovery operation
 */
static void discoveredDescTerminationCallBack(const CharacteristicDescriptorDiscovery::TerminationCallbackParams_t *params) {
  Serial.println("\r\n----discovery descriptor Termination");
  if(descriptor_is_found) {
    Serial.println("Open HRM notify");
    uint16_t value = 0x0001;
    ble.gattClient().write(GattClient::GATT_OP_WRITE_REQ, chars_hrm.getConnectionHandle(), desc_of_chars_hrm.getAttributeHandle(), 2, (uint8_t *)&value);
  }
}

/** @brief  write callback handle
 *
 *  @param[in] *params   params->connHandle : The handle of the connection that triggered the event
 *                       params->handle : Attribute Handle to which the write operation applies
 *                       params->writeOp : OP_INVALID               = 0x00,  // Invalid operation.
 *                                           OP_WRITE_REQ             = 0x01,  // Write request.
 *                                           OP_WRITE_CMD             = 0x02,  // Write command.
 *                                           OP_SIGN_WRITE_CMD        = 0x03,  // Signed write command.
 *                                           OP_PREP_WRITE_REQ        = 0x04,  // Prepare write request.
 *                                           OP_EXEC_WRITE_REQ_CANCEL = 0x05,  // Execute write request: cancel all prepared writes.
 *                                           OP_EXEC_WRITE_REQ_NOW    = 0x06,  // Execute write request: immediately execute all prepared writes.
 *                       params->offset : Offset for the write operation
 *                       params->len : Length (in bytes) of the data to write
 *                       params->data : Pointer to the data to write
 */
void onDataWriteCallBack(const GattWriteCallbackParams *params) {
  Serial.println("GattClient write call back ");
}

/** @brief  read callback handle
 *
 *  @param[in] *params   params->connHandle : The handle of the connection that triggered the event
 *                       params->handle : Attribute Handle to which the write operation applies
 *                       params->offset : Offset for the write operation
 *                       params->len : Length (in bytes) of the data to write
 *                       params->data : Pointer to the data to write
 */
void onDataReadCallBack(const GattReadCallbackParams *params) {
  Serial.println("GattClient read call back ");
  Serial.print("The handle : ");
  Serial.println(params->handle, HEX);
  Serial.print("The offset : ");
  Serial.println(params->offset, DEC);
  Serial.print("The len : ");
  Serial.println(params->len, DEC);
  Serial.print("The data : ");
  for(uint8_t index=0; index<params->len; index++) {
    Serial.print( params->data[index], HEX);
  }
  Serial.println("");
}

/** @brief  hvx callback handle
 *
 *  @param[in] *params   params->connHandle : The handle of the connection that triggered the event
 *                       params->handle : Attribute Handle to which the write operation applies
 *                       params->type : BLE_HVX_NOTIFICATION = 0x01
 *                                      BLE_HVX_INDICATION   = 0x02
 *                       params->len : Length (in bytes) of the data to write
 *                       params->data : Pointer to the data to write
 */
void hvxCallBack(const GattHVXCallbackParams *params) {
  Serial.println("GattClient notify call back ");
  Serial.print("The len : ");
  Serial.println(params->len, DEC);
  for(unsigned char index=0; index<params->len; index++) {
    Serial.print(params->data[index], HEX);
  }
  Serial.println("");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("BLE Central Demo ");

  ble.init();
  ble.onConnection(connectionCallBack);
  ble.onDisconnection(disconnectionCallBack);
  ble.gattClient().onServiceDiscoveryTermination(discoveryTerminationCallBack);
  ble.gattClient().onHVX(hvxCallBack);
  ble.gattClient().onDataWrite(onDataWriteCallBack);
  ble.gattClient().onDataRead(onDataReadCallBack);
  // scan interval : in milliseconds, valid values lie between 2.5ms and 10.24s
  // scan window :in milliseconds, valid values lie between 2.5ms and 10.24s
  // timeout : in seconds, between 0x0001 and 0xFFFF, 0x0000 disables timeout
  // activeScanning : true or false
  ble.setScanParams(1000, 200, 0, false);
  // start scanning
  ble.startScan(scanCallBack);
}

void loop() {
  // put your main code here, to run repeatedly:
  ble.waitForEvent();
}

