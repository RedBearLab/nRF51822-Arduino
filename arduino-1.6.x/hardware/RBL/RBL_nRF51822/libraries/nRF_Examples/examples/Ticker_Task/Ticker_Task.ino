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
  This example show how to use timer task.
*/

Ticker ticker1, ticker2;

void m_ticker1_handle(void) {
  Serial.println("Task1 callback handle ");
}

void m_ticker2_handle(void) {
  Serial.println("Task2 callback handle ");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Ticker Task Test ");
  pinMode(D13, OUTPUT);
  // creat two task, one 1s and the other 2s
  ticker1.attach_us(m_ticker1_handle, 1000000);
  ticker2.attach(m_ticker2_handle, 2);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(D13,HIGH);
  delay(500);
  digitalWrite(D13, LOW);
  delay(500);
}

