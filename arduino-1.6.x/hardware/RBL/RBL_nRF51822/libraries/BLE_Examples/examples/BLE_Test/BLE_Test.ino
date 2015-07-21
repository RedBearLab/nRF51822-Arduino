



BLE           ble;

static uint8_t service1_uuid[]           = {0x71, 0x3D, 0, 0, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static uint8_t service1_chars1[]         = {0x71, 0x3D, 0, 2, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static uint8_t service1_chars2[]         = {0x71, 0x3D, 0, 3, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static uint8_t service1_chars3[]         = {0x71, 0x3D, 0, 4, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};

UUID service_uuid(0x180D);
UUID chars_uuid1(0x2A37);
UUID chars_uuid2(service1_chars2);
UUID chars_uuid3(service1_chars3);
    
DigitalOut    led1(P0_19);
Serial       pc(USBTX, USBRX);

static uint16_t conn_handle;

uint32_t ble_advdata_parser(uint8_t type, uint8_t advdata_len, uint8_t *p_advdata, uint8_t *len, uint8_t *p_field_data)
{
    uint8_t index=0;
    uint8_t field_length, field_type;
    
    while(index<advdata_len)
    {
        field_length = p_advdata[index];
        field_type   = p_advdata[index+1];
        if(field_type == type)
        {
            memcpy(p_field_data, &p_advdata[index+2], (field_length-1));
            *len = field_length - 1;
            return NRF_SUCCESS;
        }
        index += field_length + 1;
    }
    return NRF_ERROR_NOT_FOUND;
}

void periodicCallback(void)
{
    led1 = !led1; /* Do blinky on LED1 while we're waiting for BLE events */
}

void scanCallBack(const Gap::AdvertisementCallbackParams_t *params)
{
    pc.printf("Scan CallBack \r\n");
    pc.printf("PerrAddress: ");
    for(uint8_t index=0; index<6; index++)
    {
       pc.printf("%x ", params->peerAddr[index]);  
    }   
    pc.printf("\r\n");  
    
    pc.printf("The Rssi : %d \r\n", params->rssi);
    
    pc.printf("The adv_data : %s \r\n", params->advertisingData);
    
    uint8_t len;
    uint8_t adv_name[31];
    if( NRF_SUCCESS == ble_advdata_parser(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, params->advertisingDataLen, (uint8_t *)params->advertisingData, &len, adv_name) )
    {
        pc.printf("Short name len : %d \r\n", len);
        pc.printf("Short name is %s \r\n", adv_name);  
        if( (len == 4) && (memcmp("TXRX", adv_name, len) == 0x00) )
        {
            pc.printf("Got device, stop scan \r\n");    
            ble.stopScan();
            ble.connect(params->peerAddr, Gap::ADDR_TYPE_RANDOM_STATIC, NULL, NULL);
        }
        
    }
    else if( NRF_SUCCESS == ble_advdata_parser(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, params->advertisingDataLen, (uint8_t *)params->advertisingData, &len, adv_name) )
    {
        pc.printf("Long name len : %d \r\n", len);
        pc.printf("Long name is %s \r\n", adv_name);  
        if( (len == 5) && (memcmp("HRM1", adv_name, len) == 0x00) )
        {
            pc.printf("Got device, stop scan \r\n");    
            ble.stopScan();
            ble.connect(params->peerAddr, Gap::ADDR_TYPE_RANDOM_STATIC, NULL, NULL);
        }
    }
    pc.printf("\r\n");   
}

void ServiceCallBack(const DiscoveredService *service)
{
    pc.printf("Servuce Discovered............ \r\n");      
    pc.printf("UUID : %2x \r\n", service->getUUID().getShortUUID());    
}

void CharacteristicCallBack(const DiscoveredCharacteristic *chars)
{
    pc.printf("Characteristic Discovered........... \r\n");   
    pc.printf("properties_broadcast   : %d \r\n", chars->getProperties()._broadcast);
    pc.printf("properties_read        : %d \r\n", chars->getProperties()._read);
    pc.printf("properties_writeWoResp : %d \r\n", chars->getProperties()._writeWoResp);
    pc.printf("properties_write       : %d \r\n", chars->getProperties()._write);
    pc.printf("properties_notify      : %d \r\n", chars->getProperties()._notify);    

    pc.printf("declHandle             : %2x \r\n", chars->getDeclHandle());  
    pc.printf("valueHandle             : %2x \r\n", chars->getValueHandle());  

     pc.printf("descHandle             : %2x \r\n", chars->getDescHandle());  
    pc.printf("CCCDHandle             : %2x \r\n", chars->getCCCDHndle());     

    uint16_t value = 0x0001;
    //ble.gattClient().read(conn_handle, chars->getValueHandle(), 0);
    //ble.gattClient().write(GattClient::GATT_OP_WRITE_CMD,conn_handle,chars->getValueHandle(),2,(uint8_t *)&value);
    ble.gattClient().write(GattClient::GATT_OP_WRITE_REQ,conn_handle,chars->getCCCDHndle(),2,(uint8_t *)&value);
}

 // GAP call back handle
static void connectionCallBack( const Gap::ConnectionCallbackParams_t *params )
{
    pc.printf("GAP_EVT_CONNECTED");    
    pc.printf("The conn handle : %2x \r\n", params->handle);

    pc.printf("PeerAddr type is : %x", params->peerAddrType);
    pc.printf("PeerAddr is : ");
    for(uint8_t index=0; index<6; index++)
    {
        pc.printf("%x ", params->peerAddr[index]);  
    }
    pc.printf("\r\n");  
    
    conn_handle = params->handle;
    ble.gattClient().launchServiceDiscovery(params->handle, ServiceCallBack, CharacteristicCallBack, service_uuid, chars_uuid1);
}

static void disconnectionCallBack(Gap::Handle_t handle, Gap::DisconnectionReason_t reason)
{
    pc.printf("Disconnected \r\n");
}

static void discoveryTermination(Gap::Handle_t connectionHandle)
{
    pc.printf("discoveryTermination............ \r\n");    
}

static void onDataWrite(const GattWriteCallbackParams *params)
{
    pc.printf("GattClient write call back \r\n");    
}

static void onDataRead(const GattReadCallbackParams *params)
{
    pc.printf("GattClient read call back \r\n");      
    pc.printf("The handle : %2x \r\n", params->handle);
    pc.printf("The offset : %d \r\n", params->offset);
    pc.printf("The len : %2x \r\n", params->len);
    pc.printf("The data : ");
    for(uint8_t index=0; index<params->len; index++)
    {
        pc.printf("%X ", params->data[index]);    
    }
    pc.printf("\r\n");
}

static void hvxCallBack(const GattHVXCallbackParams *params)
{
    pc.printf("GattClient notify call back \r\n");  
    pc.printf("The len : %d \r\n", params->len);
    for(unsigned char index=0; index<params->len; index++)
    {
        pc.printf("%d ", params->data[index]);  
    }
    pc.printf("\r\n");
}

int main(void)
{
    pc.baud(9600);
    wait(8);
    pc.printf("BLE Scan \r\n");
    
    led1 = 1;
    Ticker ticker;
    ticker.attach(periodicCallback, 1); // blink LED every second

    ble.init();
    ble.onConnection(connectionCallBack);
    ble.onDisconnection(disconnectionCallBack);
    ble.gattClient().onServiceDiscoveryTermination(discoveryTermination);
    ble.gattClient().onHVX(hvxCallBack);
    ble.gattClient().onDataWrite(onDataWrite);
    ble.gattClient().onDataRead(onDataRead);
    
    ble.startScan(scanCallBack);
    

    while(1)
    {
          
    }
  
}
