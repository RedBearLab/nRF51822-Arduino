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
#include <BLE_API.h>

#define DEVICE_NAME          "RGB"
#define TXRX_BUF_LEN         20

#define RGB_B_PIN            D2
#define RGB_G_PIN            D1
#define RGB_R_PIN            D0
#define BUTTON_PIN           D3
#define DEVICE_ID            0xFF

BLE                          ble;

// The uuid of service and characteristics
static const uint8_t service1_uuid[]         = {0x5A, 0x2D, 0x3B, 0xF8, 0xF0, 0xBC, 0x11, 0xE5, 0x9C, 0xE9, 0x5E, 0x55, 0x17, 0x50, 0x7E, 0x66};
static const uint8_t service1_chars1_uuid[]  = {0x5A, 0x2D, 0x40, 0xEE, 0xF0, 0xBC, 0x11, 0xE5, 0x9C, 0xE9, 0x5E, 0x55, 0x17, 0x50, 0x7E, 0x66};
static const uint8_t service1_chars2_uuid[]  = {0x5A, 0x2D, 0x42, 0x9C, 0xF0, 0xBC, 0x11, 0xE5, 0x9C, 0xE9, 0x5E, 0x55, 0x17, 0x50, 0x7E, 0x66};
static const uint8_t uart_base_uuid_rev[]    = {0x66, 0x7E, 0x50, 0x17, 0x55, 0x5E, 0xE9, 0x9C, 0xE5, 0x11, 0xBC, 0xF0, 0xF8, 0x3B, 0x2D, 0x5A};

uint8_t chars1_value[TXRX_BUF_LEN] = {0};
uint8_t chars2_value[TXRX_BUF_LEN] = {0};

// Create characteristic and service
GattCharacteristic  characteristic1(service1_chars1_uuid, chars1_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE );
GattCharacteristic  characteristic2(service1_chars2_uuid, chars2_value, 1, TXRX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ);
GattCharacteristic *uartChars[] = {&characteristic1, &characteristic2};
GattService         uartService(service1_uuid, uartChars, sizeof(uartChars) / sizeof(GattCharacteristic *));

static uint8_t rgb_data[3]={0xFF, 0xFF, 0xFF};
static uint8_t rgb_status=0;


void disconnectionCallBack(const Gap::DisconnectionCallbackParams_t *params) {
  //Serial.println("Disconnected ");
  //Serial.println("Restart advertising ");
  ble.startAdvertising();
}

void gattServerWriteCallBack(const GattWriteCallbackParams *Handler) {
  uint8_t buf[TXRX_BUF_LEN];
  uint16_t bytesRead, index;

  //Serial.println("Write Handle : ");
  if (Handler->handle == characteristic1.getValueAttribute().getHandle()) {
    ble.readCharacteristicValue(characteristic1.getValueAttribute().getHandle(), buf, &bytesRead);
    //for(index=0; index<bytesRead; index++)
    //{
        //Serial.print(buf[index], HEX);
    //}
    //Serial.println(" ");
    if( (buf[0] != 0x00) || (buf[1] != 0x00) || (buf[2] != 0x00) ) {
      // Update RGB's value
      rgb_status = 1;
      analogWrite(RGB_R_PIN, (255 - buf[0]));
      analogWrite(RGB_G_PIN, (255 - buf[1]));
      analogWrite(RGB_B_PIN, (255 - buf[2]));
      rgb_data[0] = buf[0];
      rgb_data[1] = buf[1];
      rgb_data[2] = buf[2];
      ble.updateCharacteristicValue(characteristic2.getValueAttribute().getHandle(), buf, 3, true);
    }
    else {
      // Make RGB off
      rgb_status = 0;
      analogWrite(RGB_R_PIN, 255);
      analogWrite(RGB_G_PIN, 255);
      analogWrite(RGB_B_PIN, 255);
      ble.updateCharacteristicValue(characteristic2.getValueAttribute().getHandle(), buf, 3, true);
    }
  }
}

void button_handle() {
  // Button event will toggle RGB's state between ON and OFF
  uint8_t buf[3];

  if(rgb_status == 0) {
    rgb_status = 1;
    analogWrite(RGB_R_PIN, (255 - rgb_data[0]));
    analogWrite(RGB_G_PIN, (255 - rgb_data[1]));
    analogWrite(RGB_B_PIN, (255 - rgb_data[2]));
    buf[0] = rgb_data[0];
    buf[1] = rgb_data[1];
    buf[2] = rgb_data[2];
  }
  else {
    rgb_status = 0;
    analogWrite(RGB_R_PIN, 255);
    analogWrite(RGB_G_PIN, 255);
    analogWrite(RGB_B_PIN, 255);
    buf[0] = 0x00;
    buf[1] = 0x00;
    buf[2] = 0x00;
  }
  ble.updateCharacteristicValue(characteristic2.getValueAttribute().getHandle(), buf, 3);
}

void setup() {
  // put your setup code here, to run once
  //Serial.begin(9600);
  //Serial.println("Start ");
  pinMode(D13, OUTPUT);

  analogWrite(RGB_R_PIN, 0);
  analogWrite(RGB_G_PIN, 0);
  analogWrite(RGB_B_PIN, 0);
  attachInterrupt(BUTTON_PIN, button_handle, FALLING);

  ble.init();
  ble.onDisconnection(disconnectionCallBack);
  ble.onDataWritten(gattServerWriteCallBack);
  // setup adv_data and srp_data
  ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED);
  ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,
                                   (const uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME) - 1);
  ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,
                                   (const uint8_t *)uart_base_uuid_rev, sizeof(uart_base_uuid_rev));
  // set adv_type
  ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
  // add service
  ble.addService(uartService);
  // set device name
  ble.setDeviceName((const uint8_t *)DEVICE_NAME);
  // set tx power,valid values are -40, -20, -16, -12, -8, -4, 0, 4
  ble.setTxPower(4);
  // set adv_interval, 100ms in multiples of 0.625ms.
  ble.setAdvertisingInterval(160);
  // set adv_timeout, in seconds
  ble.setAdvertisingTimeout(0);
  // set RGB default status
  uint8_t buf[3] = {0x00, 0x00, 0x00};
  analogWrite(RGB_R_PIN, 255);
  analogWrite(RGB_G_PIN, 255);
  analogWrite(RGB_B_PIN, 255);
  ble.updateCharacteristicValue(characteristic2.getValueAttribute().getHandle(), buf, 3, true);
  memset(rgb_data, 0xFF, 3);
  // start advertising
  ble.startAdvertising();
  //Serial.println("start advertising ");
}

void loop() {
  // put your main code here, to run repeatedly:
  ble.waitForEvent();
}

