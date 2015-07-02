/*
    Copyright (c) 2014 RedBearLab, All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
/*    nRF51822                    UNO
*Example:
*     mast_write with softdevice  slave_sender
*Pin:
*     SCL                         SCL
*     SDA                         SDA
*address:
*     0x02                        0x02
*read_len:
      0x06
*/

#include <rbl_ble.h>
#include <Wire.h>

#define TXRX_BUF_LEN                     20
#define TWI_TIME                         APP_TIMER_TICKS(1000, 0)

static app_timer_id_t                    m_twi_id;
// The Nordic UART Service
const ble_uuid128_t    base_uuid = {0x1E, 0x94, 0x8D, 0xF1, 0x48, 0x31, 0x94, 0xBA, 0x75, 0x4C, 0x3E, 0x50, 0, 0, 0x3D, 0x71};
uint16_t               uart_service_uuid = 0x0000;
uint16_t               tx_char_uuid      = 0x0003;
uint16_t               rx_char_uuid      = 0x0002;

uint8_t                tx_buf[TXRX_BUF_LEN];
uint8_t                rx_buf[TXRX_BUF_LEN];

UUID                   uartServiceUUID(base_uuid, uart_service_uuid);
UUID                   txCharUUID(base_uuid, tx_char_uuid);
UUID                   rxCharUUID(base_uuid, rx_char_uuid);

Characteristic         txChar(txCharUUID, tx_buf, 1, TXRX_BUF_LEN, 
                              Characteristic::BLE_GATT_CHAR_PROPERTIES_WRITE |
                              Characteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE |
                              Characteristic::BLE_GATT_CHAR_PROPERTIES_READ);

Characteristic         rxChar(rxCharUUID, rx_buf, 1, TXRX_BUF_LEN,
                              Characteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

Characteristic         *uartChars[] = {&txChar, &rxChar};
Service                uartService(uartServiceUUID, uartChars, sizeof(uartChars) / sizeof(Characteristic *));

void uartCallBack(void)
{
    static uint8_t buf[TXRX_BUF_LEN];
    static uint8_t rx_buf_num;
    
    while( Serial.available() )
    {
        buf[rx_buf_num++] = Serial.read();
        if(rx_buf_num >= TXRX_BUF_LEN || buf[rx_buf_num-1] == '\0' ||  buf[rx_buf_num-1] == '\n')
        {
            ble.updateCharValue(rxChar.getHandle().value_handle,buf, rx_buf_num);
            rx_buf_num = 0;
            break;
        }
    }
}

void onConnCB(uint16_t conn_handle)
{
    Serial.println("Connected! \r\n");
}

void onDisconnCB(uint16_t conn_handle, uint8_t reason)
{
    Serial.println("Disconnected! \r\n");
    Serial.println("Restarting the advertising process \r\n");
    ble.startAdvertising();
}

void onDataWriteCB(uint16_t write_handle, uint8_t data_buf[], uint16_t data_len)
{	
    if (write_handle == txChar.getHandle().value_handle) 
    {
        Serial.println("onDataWritten()");
        
        Serial.print("data_len: ");
        Serial.println(data_len, HEX);
        for(uint8_t i=0; i<data_len; i++)
        {
            Serial.write(data_buf[i]);
        }
        Serial.println("");
        memset(tx_buf, 0, TXRX_BUF_LEN);
        memcpy(tx_buf, data_buf, TXRX_BUF_LEN);	
    }
}

void m_twi_handle(void * p_context)
{
    Wire.requestFrom(0x02,6);
    while( Wire.available() > 0 )
    {
        uint8_t c = Wire.read();
        Serial.write(c);
    }
}

void setup(void)
{
    uint32_t err_code = NRF_SUCCESS;
    uart_callback_t uart_cb;
    
    Serial.begin(115200);
    Serial.println("Hello ");
    
    Serial.println("Initialising the nRF51822 ");
    uart_cb = &uartCallBack;
    Serial.irq_attach(uart_cb);
    
    ASSERT_STATUS( ble.init() );
    ASSERT_STATUS( ble.register128bitUUID(base_uuid) );
    
    ble.registerPeriphConnCB(onConnCB);
    ble.registerPeriphDisconnCB(onDisconnCB);
    ble.registerPeriphOnDataWriteCB(onDataWriteCB);

    ASSERT_STATUS( ble.addService(uartService) );
    
    // setup advertising 
    ble.accumulateAdvertisingPayload(AdvertisingData::BREDR_NOT_SUPPORTED);
    ble.accumulateAdvertisingPayload(AdvertisingData::COMPLETE_LOCAL_NAME,
                                    (const uint8_t *)"BLE_UART", sizeof("BLE_UART") - 1);
    ble.accumulateAdvertisingPayload(AdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,
                                    (const uint8_t *)&base_uuid, sizeof(base_uuid));
    ble.setAdvertisingType(AdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    //100ms; in multiples of 0.625ms
    ble.setAdvertisingInterval(160); 
    /* Set advertising data */
    ble.loadAdvertisingData();
    /* Set device name */
    ble.setGapDeviceName("SimpleChat");
     
    delay(500);
    ASSERT_STATUS( ble.startAdvertising() );
    
    //after initialize softdevice 
    Wire.begin(TWI_SCL, TWI_SDA, TWI_FREQUENCY_FREQUENCY_K100);
    
    err_code = app_timer_create(&m_twi_id,APP_TIMER_MODE_REPEATED, m_twi_handle);
    APP_ERROR_CHECK(err_code);
    
    err_code = app_timer_start(m_twi_id, TWI_TIME, NULL);
    APP_ERROR_CHECK(err_code);
    
    Serial.println("Advertising Start!");

}

void loop(void)
{

}
