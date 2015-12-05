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

#include <Wire.h>

#define DEV_ADDR  (0xA0>>1)

static uint8_t wt_data[10] = {'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
static uint8_t rd_data[10];

void AT24C512_WriteBytes(uint16_t addr, uint8_t *pbuf, uint16_t len)
{
    Wire.beginTransmission(DEV_ADDR);
    Wire.write((uint8_t)addr>>8);
    Wire.write((uint8_t)addr);
    Wire.write(pbuf, len);
    Wire.endTransmission();
}

void AT24C512_ReadBytes(uint16_t addr, uint8_t *pbuf, uint16_t len)
{
    Wire.beginTransmission(DEV_ADDR);
    Wire.write((uint8_t)addr>>8);
    Wire.write((uint8_t)addr);
    Wire.endTransmission();

    Wire.requestFrom(DEV_ADDR,len);
    while( Wire.available() > 0 )
    {
        *pbuf = Wire.read();
        pbuf++;
    }
}

void setup() {
    // put your setup code here, to run once:
    pinMode(D13, OUTPUT);
    Serial.begin(115200);
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


