
#include "HeartRateService.h"
#include "DeviceInformationService.h"
#include "ble/ServiceDiscovery.h"
#include "ble/DiscoveredCharacteristic.h"
#include "ble_advdata_parser.h"
#include "ble_gap.h"


BLE           ble;
UUID service_uuid((uint16_t)0x0001);
UUID chars_uuid1((uint16_t)0x0002);
UUID chars_uuid2((uint16_t)0x0003);
UUID chars_uuid3((uint16_t)0x0000);
    
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
        pc.printf("Short name len : %d \r\n", len);
        pc.printf("Long name is %s \r\n", adv_name);  
    }
    pc.printf("\r\n");   
}

void ServiceCallBack(const DiscoveredService *service)
{
    pc.printf("Servuce Discovered............ \r\n");      
        
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
    pc.printf("declHandle             : %2x \r\n", chars->getValueHandle());  

    uint16_t value = 0x0102;
    ble.gattClient().read(conn_handle, chars->getValueHandle(), 0);
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
    ble.gattClient().launchServiceDiscovery(params->handle, ServiceCallBack, NULL, service_uuid, chars_uuid3);
}

static void disconnectionCallBack(Gap::Handle_t handle, Gap::DisconnectionReason_t reason)
{
    pc.printf("Disconnected \r\n");
}

static void discoveryTermination(Gap::Handle_t connectionHandle)
{
    pc.printf("discoveryTermination \r\n");    
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
        pc.printf("%d ", params->data[index]);    
    }
    pc.printf("\r\n");
}

static void hvxCallBack(const GattHVXCallbackParams *params)
{
    pc.printf("GattClient notify call back \r\n");  
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
