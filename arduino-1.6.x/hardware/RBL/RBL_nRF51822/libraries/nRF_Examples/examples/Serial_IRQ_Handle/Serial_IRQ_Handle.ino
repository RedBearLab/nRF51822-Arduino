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

// handle when rx event
void uart_handle(uint32_t id, SerialIrq event) {
  // Serial receive byte event
  if(event == RxIrq){
    // print the receive byte
    if(Serial.available())
      Serial.write(Serial.read());
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.attach(uart_handle);
  Serial.println("Serial IRQ Handle demo ");
  Serial.println("Please input string to Serial com :");
  pinMode(D13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(D13,HIGH);
  delay(1000);
  digitalWrite(D13, LOW);
  delay(1000);
}

