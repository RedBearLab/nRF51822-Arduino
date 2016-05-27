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
#include <Servo.h>

Servo myservo[4];

int pos = 0;

void setup() {
  // put your setup code here, to run once
  myservo[0].attach(D2);
  myservo[1].attach(D3);
  myservo[2].attach(D4);
  myservo[3].attach(D5);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(pos = 0; pos < 180; pos += 1) {
    myservo[0].write(pos);
    myservo[1].write(pos);
    myservo[2].write(pos);
    myservo[3].write(pos);
    delay(15);
  }
  for(pos = 180; pos >= 1; pos -= 1) {
    myservo[0].write(pos);
    myservo[1].write(pos);
    myservo[2].write(pos);
    myservo[3].write(pos);
    delay(15);
  }
}

