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

/*
	When using the Serial Monitor in Arduino IDE, please set "Newline" and "9,600 baud rate".
*/

#include <BLE_API.h>

#define TXRX_BUF_LEN                     20

BLEDevice  ble;

// The Nordic UART Service
static const uint8_t uart_base_uuid[]     =   {0x71, 0x3D, 0, 0, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t uart_tx_uuid[]       =   {0x71, 0x3D, 0, 3, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t uart_rx_uuid[]       =   {0x71, 0x3D, 0, 2, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t uart_base_uuid_rev[] =   {0x1E, 0x94, 0x8D, 0xF1, 0x48, 0x31, 0x94, 0xBA, 0x75, 0x4C, 0x3E, 0x50, 0, 0, 0x3D, 0x71};
uint8_t txPayload[TXRX_BUF_LEN]           =   {0,};
uint8_t rxPayload[TXRX_BUF_LEN]           =   {0,};

//creat tx characteristic
GattCharacteristic  txCharacteristic (uart_tx_uuid, txPayload, 1, TXRX_BUF_LEN,
                                      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE);
//creat rx characteristic                                   
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
        //you must keep this line or use a delay function to instead of it.
        //because you need to wait for transmission complete
        Serial.println("onDataWritten()");
        
        ble.readCharacteristicValue(txCharacteristic.getHandle(), buf, &bytesRead);
        Serial.print("bytesRead: ");
        Serial.println(bytesRead, HEX);
        for(uint8_t i=0; i<bytesRead; i++)
        {
            Serial.write(buf[i]);
        }
        Serial.println("");
    }
}

void setup(void)
{
    uint32_t err_code = NRF_SUCCESS;
    
    delay(500);
    Serial.begin(9600);
    Serial.println("Initialising the nRF51822\n\r");

    Serial.irq_attach(uartCallBack);
    
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
    
    //Set Dev_Name
    err_code = RBL_SetDevName("SimpleChat");
    APP_ERROR_CHECK(err_code);
    
    ble.startAdvertising();
    
    Serial.println("Advertising Start!");
}

void loop(void)
{   //Low Power, 
    ble.waitForEvent();
}
