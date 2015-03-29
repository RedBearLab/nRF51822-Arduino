
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
/*    BLE_Nano                    UNO
*Example:
*     mast_write              slave_receiver
*Pin:
*     D6                          SCL
*     D5                          SDA
*address:
*     0x04                        0x04
*/
#include <Wire.h>

void setup() 
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    //Wire.begin();
    Wire.begin(D6, D5, TWI_FREQUENCY_FREQUENCY_K100);
    Serial.println("master write...");
}

byte x = 0;

void loop() 
{
    // put your main code here, to run repeatedly: 
    Serial.println("master send begin: ");
    //For example: slave_receiver example fo UNO on Arduino1.0.5, the address is #4, So transmission address is 0x08;
    Wire.beginTransmission(0x04); 
    Wire.write("x is");
    Wire.write(x);
    Wire.endTransmission();
    
    x++;
    delay(1000);
}
