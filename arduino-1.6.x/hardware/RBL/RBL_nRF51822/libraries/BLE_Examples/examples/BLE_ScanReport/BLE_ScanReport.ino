#include <BLE_API.h>

BLE           ble;

uint32_t ble_advdata_decode(uint8_t type, uint8_t advdata_len, uint8_t *p_advdata, uint8_t *len, uint8_t *p_field_data)
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
     Serial.println("Scan Device CallBack Handle ");

     Serial.print("  The peerAddr : ");
     for(uint8_t index=0; index<6; index++)
     {
         Serial.print(params->peerAddr[index], HEX);
         Serial.print(" ");
     }
     Serial.println(" ");

     Serial.print("  The Rssi : ");
     Serial.println(params->rssi, DEC);

     uint8_t len;
     uint8_t adv_name[31];
     if( NRF_SUCCESS == ble_advdata_decode(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, params->advertisingDataLen, (uint8_t *)params->advertisingData, &len, adv_name) )
     {
         Serial.print("  The length of Short Local Name : ");
         Serial.println(len, HEX);
         Serial.print("  The Short Local Name is        : ");
         Serial.println((const char *)adv_name);
     }
     else if( NRF_SUCCESS == ble_advdata_decode(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, params->advertisingDataLen, (uint8_t *)params->advertisingData, &len, adv_name) )
     {
         Serial.print("  The length of Complete Local Name : ");
         Serial.println(len, HEX);
         Serial.print("  The Complete Local Name is        : ");
         Serial.println((const char *)adv_name);
     }
     Serial.println(" ");
     Serial.println(" ");
}

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println("Start...");
    ble.init();
    //Note : take care of scheduler, prevent ram leak.See projectconfig.h
    ble.startScan(scanCallBack);
}

void loop()
{
    // put your main code here, to run repeatedly:
    ble.waitForEvent();
}

