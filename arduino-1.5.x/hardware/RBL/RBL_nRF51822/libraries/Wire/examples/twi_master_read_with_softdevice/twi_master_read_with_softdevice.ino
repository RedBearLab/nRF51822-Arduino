
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
#include <BLE_API.h>
#include <Wire.h>

#define TXRX_BUF_LEN                      20
#define TWI_TIME                          APP_TIMER_TICKS(1000, 0)

BLEDevice  ble;

static app_timer_id_t                     m_twi_id;
// The Nordic UART Service
static const uint8_t uart_base_uuid[]     = {0x71, 0x3D, 0, 0, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t uart_tx_uuid[]       = {0x71, 0x3D, 0, 3, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t uart_rx_uuid[]       = {0x71, 0x3D, 0, 2, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t uart_base_uuid_rev[] = {0x1E, 0x94, 0x8D, 0xF1, 0x48, 0x31, 0x94, 0xBA, 0x75, 0x4C, 0x3E, 0x50, 0, 0, 0x3D, 0x71};
uint8_t txPayload[TXRX_BUF_LEN]           = {0,};
uint8_t rxPayload[TXRX_BUF_LEN]           = {0,};
GattCharacteristic  txCharacteristic (uart_tx_uuid, txPayload, 1, TXRX_BUF_LEN,
                                      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE);
GattCharacteristic  rxCharacteristic (uart_rx_uuid, rxPayload, 1, TXRX_BUF_LEN,
                                      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);
GattCharacteristic *uartChars[] = {&txCharacteristic, &rxCharacteristic};
GattService         uartService(uart_base_uuid, uartChars, sizeof(uartChars) / sizeof(GattCharacteristic *));

void uartCallBack(void)
{
    static uint8_t buf[TXRX_BUF_LEN];
    static uint8_t rx_buf_num;
    
    while( Serial.available() )
    {
        buf[rx_buf_num++] = Serial.read();
        if(rx_buf_num >= TXRX_BUF_LEN || buf[rx_buf_num-1] == '\0' ||  buf[rx_buf_num-1] == '\n')
        {
            ble.updateCharacteristicValue(rxCharacteristic.getHandle(), buf, rx_buf_num);
            rx_buf_num = 0;
            break;
        } 
    }
}

void disconnectionCallback(void)
{
    Serial.println("Disconnected! \r\n");
    Serial.println("Restarting the advertising process \r\n");
    ble.startAdvertising();
}

void onDataWritten(uint16_t charHandle)
{	
    uint8_t buf[TXRX_BUF_LEN];
    uint16_t bytesRead;
	
    if (charHandle == txCharacteristic.getHandle()) 
    {
        Serial.println("onDataWritten()");
        ble.readCharacteristicValue(txCharacteristic.getHandle(), buf, &bytesRead);
        Serial.print("bytesRead: ");
        Serial.println(bytesRead, HEX);
        for(uint8_t i=0; i<bytesRead; i++)
        {
            Serial.write(buf[i]);
        }
        Serial.println("");
        memset(txPayload, 0, TXRX_BUF_LEN);
        memcpy(txPayload, buf, TXRX_BUF_LEN);		
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
    
    delay(500);
    Serial.begin(9600);
    Serial.println("Initialising the nRF51822\n\r");
    
    uart_cb = &uartCallBack;
    Serial.irq_attach(uart_cb);
    
    ble.init();
    ble.onDisconnection(disconnectionCallback);
    ble.onDataWritten(onDataWritten);

    // setup advertising 
    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED);
    ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,
                                    (const uint8_t *)"Biscuit", sizeof("Biscuit") - 1);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,
                                    (const uint8_t *)uart_base_uuid_rev, sizeof(uart_base_uuid));
    // 100ms; in multiples of 0.625ms
    ble.setAdvertisingInterval(160); 

    ble.addService(uartService);
    
    ble.startAdvertising();
    
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
    ble.waitForEvent();
}
