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
#include <Servo.h>
#include "pin_inf.h"
#include <BLE_API.h>

#define PIN_CAPABILITY_NONE              0x00
#define PIN_CAPABILITY_DIGITAL           0x01
#define PIN_CAPABILITY_ANALOG            0x02
#define PIN_CAPABILITY_PWM               0x04
#define PIN_CAPABILITY_SERVO             0x08

#define ANALOG                           0x02 // analog pin in analogInput mode
#define PWM                              0x03 // digital pin in PWM output mode
#define SERVO                            0x04 // digital pin in Servo output mode

#define TXRX_BUF_LEN                     20

#define STATUS_CHECK_TIME                APP_TIMER_TICKS(30, 0)
#define STATUS_REBACK_TIME               APP_TIMER_TICKS(100, 0)

BLEDevice  ble;
//for BLEController
byte pin_mode[TOTAL_PINS];
byte pin_state[TOTAL_PINS];
byte pin_pwm[TOTAL_PINS];
byte pin_servo[TOTAL_PINS];

Servo servos[MAX_SERVOS];

static byte queryDone            = false;
static byte buf_len              = 0;
static uint8_t reback_pin        = 2;
static uint8_t status_check_flag = 1;

static app_timer_id_t                       m_status_check_id; 
static app_timer_id_t                       m_status_reback_id; 

// The Nordic UART Service
static const uint8_t uart_base_uuid[]       = {0x71, 0x3D, 0, 0, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t uart_tx_uuid[]         = {0x71, 0x3D, 0, 3, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t uart_rx_uuid[]         = {0x71, 0x3D, 0, 2, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t uart_base_uuid_rev[]   = {0x1E, 0x94, 0x8D, 0xF1, 0x48, 0x31, 0x94, 0xBA, 0x75, 0x4C, 0x3E, 0x50, 0, 0, 0x3D, 0x71};

uint8_t txPayload[TXRX_BUF_LEN] = {0,};
uint8_t rxPayload[TXRX_BUF_LEN] = {0,};

GattCharacteristic  txCharacteristic (uart_tx_uuid, txPayload, 1, TXRX_BUF_LEN,
                                      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE);
                                      
GattCharacteristic  rxCharacteristic (uart_rx_uuid, rxPayload, 1, TXRX_BUF_LEN,
                                      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);
                                      
GattCharacteristic *uartChars[] = {&txCharacteristic, &rxCharacteristic};

GattService         uartService(uart_base_uuid, uartChars, sizeof(uartChars) / sizeof(GattCharacteristic *));


void disconnectionCallback(void)
{
    ble.startAdvertising();
}

void reportPinDigitalData(byte pin)
{
    byte state = digitalRead(pin);
    byte mode = pin_mode[pin];
    byte buf[] = {'G', pin, mode, state};         
    ble.updateCharacteristicValue(rxCharacteristic.getHandle(), buf, 4);
}

void reportPinPWMData(byte pin)
{
    byte value = pin_pwm[pin];
    byte mode = pin_mode[pin];
    byte buf[] = {'G', pin, mode, value};         
    ble.updateCharacteristicValue(rxCharacteristic.getHandle(), buf, 4);
}

void reportPinServoData(byte pin)
{
    byte value = pin_servo[pin];
    byte mode = pin_mode[pin];
    byte buf[] = {'G', pin, mode, value};         
    ble.updateCharacteristicValue(rxCharacteristic.getHandle(), buf, 4);
}

void reportPinCapability(byte pin)
{
    byte buf[] = {'P', pin, 0x00};
    byte pin_cap = 0;
                      
    if (IS_PIN_DIGITAL(pin))
        pin_cap |= PIN_CAPABILITY_DIGITAL;
              
    if (IS_PIN_ANALOG(pin))
        pin_cap |= PIN_CAPABILITY_ANALOG;
  
    if (IS_PIN_PWM(pin))
        pin_cap |= PIN_CAPABILITY_PWM;
  
    if (IS_PIN_SERVO(pin))
        pin_cap |= PIN_CAPABILITY_SERVO;
  
    buf[2] = pin_cap;
    ble.updateCharacteristicValue(rxCharacteristic.getHandle(), buf, 3);
}

void sendCustomData(uint8_t *buf, uint8_t len)
{
    uint8_t data[20] = "Z";
    
    memcpy(&data[1], buf, len);
    ble.updateCharacteristicValue(rxCharacteristic.getHandle(), data, len+1);
}

byte reportDigitalInput()
{
    static byte pin = 0;
    byte report = 0;
    
    if (!IS_PIN_DIGITAL(pin))
    {
        pin++;
        if (pin >= TOTAL_PINS)
            pin = 0;
        return 0;
    }
    
    if (pin_mode[pin] == INPUT)
    {
        byte current_state = digitalRead(pin);
              
        if (pin_state[pin] != current_state)
        {
            pin_state[pin] = current_state;
            byte buf[] = {'G', pin, INPUT, current_state};
            ble.updateCharacteristicValue(rxCharacteristic.getHandle(), buf, 4);
          
            report = 1;
        }
    }
    pin++;
    if (pin >= TOTAL_PINS)
        pin = 0;
      
    return report;
}

byte reportPinAnalogData()
{
    static byte pin = 0;
    byte report = 0;
  
    if (!IS_PIN_DIGITAL(pin))
    {
        pin++;
        if (pin >= TOTAL_PINS)
            pin = 0;
        return 0;
    }
    
    if (pin_mode[pin] == ANALOG)
    {
        uint16_t value = analogRead(pin);
        byte value_lo = value;
        byte value_hi = value>>8;
        
        byte mode = pin_mode[pin];
        mode = (value_hi << 4) | mode;
        
        byte buf[] = {'G', pin, mode, value_lo};         
        ble.updateCharacteristicValue(rxCharacteristic.getHandle(), buf, 4);
    }
    
    pin++;
    if (pin >= TOTAL_PINS)
        pin = 0;
      
    return report;
}

void onDataWritten(uint16_t charHandle)
{	
    uint8_t buf[TXRX_BUF_LEN];
    uint16_t bytesRead;
    uint32_t err_code = NRF_SUCCESS;
    
    status_check_flag = 0;
    if (charHandle == txCharacteristic.getHandle())
    {
        ble.readCharacteristicValue(txCharacteristic.getHandle(), buf, &bytesRead);
        for(uint8_t i=0; i<bytesRead; i++)
        {    
            Serial.write(buf[i]);
        }
        switch(buf[0])
        {
            case 'V': //query protocol version
            {
                byte buf_tx[] = {'V', 0x00, 0x00, 0x01};
                ble.updateCharacteristicValue(rxCharacteristic.getHandle(), buf_tx, 4);
                break;
            }
            case 'C': // query board total pin count
            {
                byte buf_tx[2] = {'C', TOTAL_PINS};
                ble.updateCharacteristicValue(rxCharacteristic.getHandle(), buf_tx, 2);
                break;       
            }
            case 'M': // query pin mode
            {
                byte buf_tx[] = {'M', buf[1], pin_mode[ buf[2] ]};
                ble.updateCharacteristicValue(rxCharacteristic.getHandle(), buf_tx, 3);
                break;                  
            }
            case 'S': // set pin mode
            {
                byte pin = buf[1];
                byte mode = buf[2];
                if ( IS_PIN_SERVO(pin) && mode != SERVO && servos[PIN_TO_SERVO(pin)].attached() )
                    servos[PIN_TO_SERVO(pin)].detach();
                /* ToDo: check the mode is in its capability or not */
                /* assume always ok */
                if(mode != pin_mode[pin])
                {
                    pinMode(pin, mode);
                    pin_mode[pin] = mode;
                    if (mode == OUTPUT)
                    {
                        digitalWrite(pin, LOW);
                        pin_state[pin] = LOW;
                    }
                    else if (mode == INPUT)
                    {
                        digitalWrite(pin, HIGH);
                        pin_state[pin] = HIGH;
                    }
                    else if (mode == ANALOG)
                    {
                        if (IS_PIN_ANALOG(pin)) 
                        {
                            if (IS_PIN_DIGITAL(pin)) 
                            {
                                pinMode(PIN_TO_DIGITAL(pin), LOW);
                            }
                        }
                    }
                    else if (mode == PWM)
                    {
                        if (IS_PIN_PWM(pin))
                        {
                            pinMode(PIN_TO_PWM(pin), OUTPUT);
                            analogWrite(PIN_TO_PWM(pin), 0);
                            pin_pwm[pin] = 0;
                            pin_mode[pin] = PWM;
                        }
                    }
                    else if (mode == SERVO)
                    {
                        if (IS_PIN_SERVO(pin))
                        {
                            pin_servo[pin] = 0;
                            pin_mode[pin] = SERVO;
                            if (!servos[PIN_TO_SERVO(pin)].attached())
                              servos[PIN_TO_SERVO(pin)].attach(PIN_TO_DIGITAL(pin));
                        }
                    }
                }
                //if (mode == ANALOG)
                   //reportPinAnalogData(pin);
                if ( (mode == INPUT) || (mode == OUTPUT) )
                    reportPinDigitalData(pin);
                else if (mode == PWM)
                    reportPinPWMData(pin);
                else if (mode == SERVO)
                    reportPinServoData(pin);
             
                break;
            }
            case 'G': // query pin data
            {  
                byte pin = buf[1];
                reportPinDigitalData(pin);
                break;
            }
            case 'T': // set pin digital state
            {
                byte pin = buf[1];
                byte state = buf[2];
                digitalWrite(pin, state);
                reportPinDigitalData(pin);
                break;
            }
            case 'N': // set PWM
            {
                byte pin = buf[1];
                byte value = buf[2];
                
                analogWrite(PIN_TO_PWM(pin), value);
                pin_pwm[pin] = value;
                reportPinPWMData(pin);
                break;
            }
            case 'O': // set Servo
            {
                byte pin = buf[1];
                byte value = buf[2];

                if (IS_PIN_SERVO(pin))
                    servos[PIN_TO_SERVO(pin)].write(value);
                pin_servo[pin] = value;
                reportPinServoData(pin);       
                break;         
            }
            case 'A':// query all pin status
            {    
                reback_pin = 2;
                err_code = app_timer_start(m_status_reback_id, STATUS_REBACK_TIME, NULL);
                APP_ERROR_CHECK(err_code);  
                break;
            }
            case 'P':// query pin capability
            {
                byte pin = buf[1];
                reportPinCapability(pin);
                break;
            }
            case 'Z':
            {
                byte len = buf[1];
                Serial.println("->");
                Serial.print("Received: ");
                Serial.print(len);
                Serial.println(" byte(s)");
                Serial.print(" Hex: ");
                for (int i=0;i<len;i++)
                  Serial.print(buf[i+2], HEX);
                Serial.println();
                break;
            }  
        }
        buf_len = 0;	
    }
    status_check_flag = 1;
}

void m_status_check_handle(void * p_context)
{   
     if(status_check_flag)
     {
         byte input_data_pending = reportDigitalInput();
         if(input_data_pending)
         {
             buf_len = 0;
             return;
         }
         reportPinAnalogData();
         buf_len = 0;
     }
}

void m_status_reback_handle(void * p_context)
{   
    uint32_t err_code = NRF_SUCCESS;
  
    if(reback_pin < TOTAL_PINS)
    {
        reportPinCapability(reback_pin);     
        if ( (pin_mode[reback_pin] == INPUT) || (pin_mode[reback_pin] == OUTPUT) )
            reportPinDigitalData(reback_pin);
        else if (pin_mode[reback_pin] == PWM)
            reportPinPWMData(reback_pin);
        else if (pin_mode[reback_pin] == SERVO)
            reportPinServoData(reback_pin);     
     
        reback_pin++;     
    }
    else
    {
        queryDone = true;
        uint8_t str[] = "ABC";
        sendCustomData(str, 3);
        
        err_code = app_timer_stop(m_status_reback_id);
        APP_ERROR_CHECK(err_code);  
    }
}

void setup(void)
{    
    uint32_t err_code = NRF_SUCCESS;
    
    Serial.begin(115200);
    
    ble.init();
    ble.onDisconnection(disconnectionCallback);
    ble.onDataWritten(onDataWritten);

    /* setup advertising */
    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED);
    ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,
                                    (const uint8_t *)"Biscuit", sizeof("Biscuit") - 1);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,
                                    (const uint8_t *)uart_base_uuid_rev, sizeof(uart_base_uuid));
    /* 100ms; in multiples of 0.625ms. */
    ble.setAdvertisingInterval(160); 
    
    ble.addService(uartService);
    
    //Set Dev_Name
    err_code = RBL_SetDevName("BLEController");
    APP_ERROR_CHECK(err_code);
    
    ble.startAdvertising();
    
    /* Default all to digital input */
    for (int pin = 0; pin < TOTAL_PINS; pin++)
    {
        // Set pin to input with internal pull up
        pinMode(pin, INPUT);
        digitalWrite(pin, HIGH);
    
        // Save pin mode and state
        pin_mode[pin] = INPUT;
        pin_state[pin] = LOW;
    }
    
    err_code = app_timer_create(&m_status_reback_id,APP_TIMER_MODE_REPEATED, m_status_reback_handle);
    APP_ERROR_CHECK(err_code);
    
    err_code = app_timer_create(&m_status_check_id,APP_TIMER_MODE_REPEATED, m_status_check_handle);
    APP_ERROR_CHECK(err_code);
    
    err_code = app_timer_start(m_status_check_id, STATUS_CHECK_TIME, NULL);
    APP_ERROR_CHECK(err_code);   
}

void loop(void)
{
    ble.waitForEvent();
}
