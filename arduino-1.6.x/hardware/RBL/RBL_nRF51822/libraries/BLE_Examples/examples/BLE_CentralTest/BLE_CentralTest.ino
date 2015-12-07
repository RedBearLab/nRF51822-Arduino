#include <BLE_API.h>

BLE           ble;

static uint8_t service1_uuid[]           = {0x71, 0x3D, 0, 0, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static uint8_t service1_chars1[]         = {0x71, 0x3D, 0, 2, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static uint8_t service1_chars2[]         = {0x71, 0x3D, 0, 3, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static uint8_t service1_chars3[]         = {0x71, 0x3D, 0, 4, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};

UUID service_uuid(0x180D);
UUID chars_uuid1(0x2A37);
UUID chars_uuid2(service1_chars2);
UUID chars_uuid3(service1_chars3);


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

void scanCallBack(const Gap::AdvertisementCallbackParams_t *params)
{
    Serial.println("Scan CallBack ");
    Serial.print("PerrAddress: ");
    for(uint8_t index=0; index<6; index++)
    {
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
    if( NRF_SUCCESS == ble_advdata_parser(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, params->advertisingDataLen, (uint8_t *)params->advertisingData, &len, adv_name) )
    {
        Serial.print("Short name len : ");
        Serial.println(len, DEC);
        Serial.print("Short name is : ");
        Serial.println((const char*)adv_name);

        if( (len == 4) && (memcmp("TXRX", adv_name, len) == 0x00) )
        {
            Serial.println("Got device, stop scan ");
            ble.stopScan();
            ble.connect(params->peerAddr, Gap::ADDR_TYPE_RANDOM_STATIC, NULL, NULL);
        }

    }
    else if( NRF_SUCCESS == ble_advdata_parser(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, params->advertisingDataLen, (uint8_t *)params->advertisingData, &len, adv_name) )
    {
        Serial.print("Long name len : ");
        Serial.println(len, DEC);
        Serial.print("Long name is : ");
        Serial.println((const char*)adv_name);

        if( (len == 5) && (memcmp("HRM1", adv_name, len) == 0x00) )
        {
            Serial.println("Got device, stop scan ");
            ble.stopScan();
            ble.connect(params->peerAddr, Gap::ADDR_TYPE_RANDOM_STATIC, NULL, NULL);
        }
    }
    Serial.println(" ");
}

void ServiceCallBack(const DiscoveredService *service)
{
    Serial.println("Servuce Discovered............ ");
    Serial.print("Short UUID : ");
    Serial.println(service->getUUID().getShortUUID(), HEX);
}

void CharacteristicCallBack(const DiscoveredCharacteristic *chars)
{
    Serial.println("Characteristic Discovered........... \r\n");
    Serial.print("properties_read        : ");
    Serial.println(chars->getProperties()._read, DEC);
    Serial.print("properties_writeWoResp : ");
    Serial.println(chars->getProperties()._writeWoResp, DEC);
    Serial.print("properties_write       : ");
    Serial.println(chars->getProperties()._write, DEC);
    Serial.print("properties_notify      : ");
    Serial.println(chars->getProperties()._notify, DEC);

    Serial.print("valueHandle             : ");
    Serial.println(chars->getValueHandle(), HEX);
    Serial.print("CCCDHandle             : ");
    Serial.println(chars->getCCCDHndle(), HEX);

    uint16_t value = 0x0001;
    //ble.gattClient().read(chars->getConnHandle(), chars->getValueHandle(), 0);
    //ble.gattClient().write(GattClient::GATT_OP_WRITE_CMD,chars->getConnHandle(),chars->getValueHandle(),2,(uint8_t *)&value);
    ble.gattClient().write(GattClient::GATT_OP_WRITE_REQ, chars->getConnHandle(), chars->getCCCDHndle(),2,(uint8_t *)&value);
}

// GAP call back handle
void connectionCallBack( const Gap::ConnectionCallbackParams_t *params )
{
    Serial.print("GAP_EVT_CONNECTED");
    Serial.print("The conn handle : ");
    Serial.println(params->handle, HEX);

    Serial.print("  The peerAddr : ");
    for(uint8_t index=0; index<6; index++)
    {
       Serial.print(params->peerAddr[index], HEX);
       Serial.print(" ");
    }
    Serial.println(" ");

    ble.gattClient().launchServiceDiscovery(params->handle, ServiceCallBack, CharacteristicCallBack, service_uuid, chars_uuid1);
}

void disconnectionCallBack(Gap::Handle_t handle, Gap::DisconnectionReason_t reason)
{
   Serial.println("Disconnected ");
}

void discoveryTermination(Gap::Handle_t connectionHandle)
{
    Serial.println("discoveryTermination............ ");
}

void onDataWrite(const GattWriteCallbackParams *params)
{
    Serial.println("GattClient write call back ");
}

void onDataRead(const GattReadCallbackParams *params)
{
    Serial.println("GattClient read call back ");
    Serial.print("The handle : ");
    Serial.println(params->handle, HEX);
    Serial.print("The offset : ");
    Serial.println(params->offset, DEC);
    Serial.print("The len : ");
    Serial.println(params->len, DEC);
    Serial.print("The data : ");
    for(uint8_t index=0; index<params->len; index++)
    {
        Serial.print( params->data[index], HEX);
    }
    Serial.println("");
}

void hvxCallBack(const GattHVXCallbackParams *params)
{
    Serial.println("GattClient notify call back \r\n");
    Serial.print("The len : ");
    Serial.println(params->len, DEC);
    for(unsigned char index=0; index<params->len; index++)
    {
        Serial.print(params->data[index], HEX);
    }
    Serial.println("");
}

int main(void)
{
    Serial.begin(9600);
    wait(8);
    Serial.println("BLE Scan ");

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
