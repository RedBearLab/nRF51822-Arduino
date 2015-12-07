
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

Flash chip: 45DB021D

*/

#include <SPI_Master.h>

void Flash_Buff_WriteBytes(uint16_t addr, uint8_t *pbuf, uint16_t len)
{
    uint16_t index;

    digitalWrite(10, LOW);
    delayMicroseconds(200);

    SPI_Master.transfer(0x84);
    SPI_Master.transfer(0x00);
    SPI_Master.transfer( (uint8_t)(addr>>8) );
    SPI_Master.transfer( (uint8_t)addr );
    for(index=0; index<len; index++)
    {
        SPI_Master.transfer(*pbuf);
        pbuf++;
    }

    delayMicroseconds(200);
    digitalWrite(10, HIGH);
}
void Flash_Buff_ReadBytes(uint16_t addr, uint8_t *pbuf, uint16_t len)
{
    uint16_t index;

    digitalWrite(10, LOW);
    delayMicroseconds(200);

    SPI_Master.transfer(0xD1);
    SPI_Master.transfer(0x00);
    SPI_Master.transfer( (uint8_t)(addr>>8) );
    SPI_Master.transfer( (uint8_t)addr );
    for(index=0; index<len; index++)
    {
        *pbuf = SPI_Master.transfer(0x00);
        pbuf++;
    }

    delayMicroseconds(200);
    digitalWrite(10, HIGH);
}

uint8_t i;
uint8_t wt_buf[7] = {'H','e','l','l', 'o', '\r', '\n'};
uint8_t rd_buf[7];


void setup() {

    pinMode(D13, OUTPUT);
    Serial.begin(9600);
    Serial.println("SPI Start ");

    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH);
    //SPI_Master.begin();
    SPI_Master.begin(SCK, MOSI, MISO);
    SPI_Master.setFrequency(SPI_4M);

    delay(1000);
    Flash_Buff_WriteBytes(0, wt_buf, 7);
}

void loop() {
    // put your main code here, to run repeatedly:
    digitalWrite(13,HIGH);
    delay(1000);
    digitalWrite(13, LOW);
    delay(1000);

    memset(rd_buf, 0x00, 7);
    Flash_Buff_ReadBytes(0, rd_buf, 7);
    for(i=0; i<7; i++)
      Serial.write(rd_buf[i]);
}


