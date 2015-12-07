
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
#include <BLE_API.h>
#include <Servo.h>

#define TXRX_BUF_LEN                      20

#define DIGITAL_OUT_PIN                   D2
#define DIGITAL_IN_PIN                    A4
#define PWM_PIN                           D3
#define SERVO_PIN                         D5
#define ANALOG_IN_PIN                     A5


BLE                                       ble;
Ticker                                    ticker;
Servo                                     myservo;

static boolean analog_enabled = false;
static byte old_state         = LOW;

// The Nordic UART Service
static const uint8_t service1_uuid[]                = {0x71, 0x3D, 0, 0, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t service1_tx_uuid[]             = {0x71, 0x3D, 0, 3, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t service1_rx_uuid[]             = {0x71, 0x3D, 0, 2, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t uart_base_uuid_rev[]           = {0x1E, 0x94, 0x8D, 0xF1, 0x48, 0x31, 0x94, 0xBA, 0x75, 0x4C, 0x3E, 0x50, 0, 0, 0x3D, 0x71};

uint8_t tx_value[TXRX_BUF_LEN] = {0,};
uint8_t rx_value[TXRX_BUF_LEN] = {0,};

GattCharacteristic  characteristic1(service1_tx_uuid, tx_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE );

GattCharacteristic  characteristic2(service1_rx_uuid, rx_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

GattCharacteristic *uartChars[] = {&characteristic1, &characteristic2};

GattService         uartService(service1_uuid, uartChars, sizeof(uartChars) / sizeof(GattCharacteristic *));


void disconnectionCallBack(Gap::Handle_t handle, Gap::DisconnectionReason_t reason)
{
    Serial.println("Disconnected!");
    Serial.println("Restarting the advertising process");
    ble.startAdvertising();
}

void writtenHandle(const GattWriteCallbackParams *Handler)
{
    uint8_t buf[TXRX_BUF_LEN];
    uint16_t bytesRead, index;

    if (Handler->handle == characteristic1.getValueAttribute().getHandle()) {
        ble.readCharacteristicValue(characteristic1.getValueAttribute().getHandle(), buf, &bytesRead);
        Serial.print("bytesRead: ");
        Serial.println(bytesRead, HEX);
        for(byte index=0; index<bytesRead; index++) {
            Serial.write(buf[index]);
        }
        Serial.println("");
        //Process the data
        if (buf[0] == 0x01)  // Command is to control digital out pin
        {
            if (buf[1] == 0x01)
                digitalWrite(DIGITAL_OUT_PIN, HIGH);
            else
                digitalWrite(DIGITAL_OUT_PIN, LOW);
        }
        else if (buf[0] == 0xA0) // Command is to enable analog in reading
        {
            if (buf[1] == 0x01)
                analog_enabled = true;
            else
                analog_enabled = false;
        }
        else if (buf[0] == 0x02) // Command is to control PWM pin
        {
            analogWrite(PWM_PIN, buf[1]);
        }
        else if (buf[0] == 0x03)  // Command is to control Servo pin
        {
            myservo.write(buf[1]);
        }
        else if (buf[0] == 0x04)
        {
            analog_enabled = false;
            myservo.write(0);
            analogWrite(PWM_PIN, 0);
            digitalWrite(DIGITAL_OUT_PIN, LOW);
            old_state = LOW;
        }

    }
}

void m_status_check_handle()
{
    uint8_t buf[3];

    if (analog_enabled)  // if analog reading enabled
    {
        // Read and send out
        uint16_t value = analogRead(ANALOG_IN_PIN);
        buf[0] = (0x0B);
        buf[1] = (value >> 8);
        buf[2] = (value);
        ble.updateCharacteristicValue(characteristic2.getValueAttribute().getHandle(), buf, 3);
    }
    // If digital in changes, report the state
    if (digitalRead(DIGITAL_IN_PIN) != old_state)
    {
        old_state = digitalRead(DIGITAL_IN_PIN);
        if (digitalRead(DIGITAL_IN_PIN) == HIGH)
        {
            buf[0] = (0x0A);
            buf[1] = (0x01);
            buf[2] = (0x00);
            ble.updateCharacteristicValue(characteristic2.getValueAttribute().getHandle(), buf, 3);
        }
        else
        {
            buf[0] = (0x0A);
            buf[1] = (0x00);
            buf[2] = (0x00);
            ble.updateCharacteristicValue(characteristic2.getValueAttribute().getHandle(), buf, 3);
        }
    }
}

void setup() {
    // put your setup code here, to run once

    Serial.begin(9600);

    ble.init();
    ble.onDisconnection(disconnectionCallBack);
    ble.onDataWritten(writtenHandle);

    // setup adv_data and srp_data
    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,
                                     (const uint8_t *)"TXRX", sizeof("TXRX") - 1);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,
                                     (const uint8_t *)uart_base_uuid_rev, sizeof(uart_base_uuid_rev));

    // set adv_type
    ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    // add service
    ble.addService(uartService);
    // set device name
    ble.setDeviceName((const uint8_t *)"Simple Controls");
    // set tx power,valid values are -40, -20, -16, -12, -8, -4, 0, 4
    ble.setTxPower(4);
    // set adv_interval, 100ms in multiples of 0.625ms.
    ble.setAdvertisingInterval(160);
    // set adv_timeout, in seconds
    ble.setAdvertisingTimeout(0);
    // start advertising
    ble.startAdvertising();

    pinMode(DIGITAL_OUT_PIN, OUTPUT);
    pinMode(DIGITAL_IN_PIN, INPUT_PULLUP);
    pinMode(PWM_PIN, OUTPUT);

    // Default to internally pull high, change it if you need
    digitalWrite(DIGITAL_IN_PIN, HIGH);

    myservo.attach(SERVO_PIN);
    myservo.write(0);

    ticker.attach_us(m_status_check_handle, 200000);

    Serial.println("Advertising Start!");
}

void loop() {
    ble.waitForEvent();
}
