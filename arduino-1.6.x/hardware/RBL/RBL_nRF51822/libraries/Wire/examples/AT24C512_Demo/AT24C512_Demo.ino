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
#include <Wire.h>

#define DEV_ADDR  (0xA0>>1)

static uint8_t wt_data[10] = {'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
static uint8_t rd_data[10];

void AT24C512_WriteBytes(uint16_t addr, uint8_t *pbuf, uint16_t len) {
  Wire.beginTransmission(DEV_ADDR);
  Wire.write((uint8_t)addr>>8);
  Wire.write((uint8_t)addr);
  Wire.write(pbuf, len);
  Wire.endTransmission();
}

void AT24C512_ReadBytes(uint16_t addr, uint8_t *pbuf, uint16_t len) {
  Wire.beginTransmission(DEV_ADDR);
  Wire.write((uint8_t)addr>>8);
  Wire.write((uint8_t)addr);
  Wire.endTransmission();

  Wire.requestFrom(DEV_ADDR,len);
  while( Wire.available() > 0 ) {
    *pbuf = Wire.read();
    pbuf++;
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(D13, OUTPUT);
  Serial.begin(9600);
  Serial.println("Wire demo start ");

  Wire.begin();
  Serial.println("Wire demo start");
  AT24C512_WriteBytes(0, wt_data, 10);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(D13,HIGH);
  delay(500);
  digitalWrite(D13, LOW);
  delay(500);

  AT24C512_ReadBytes(0, rd_data, 10);
  Serial.println("Read data from AT24C512: ");
  Serial.write(rd_data, 10);
  Serial.print("\r\n");
}


