
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

void handle_irq1(void)
{
    Serial.println(" Handle1  ");
}

void handle_irq2(void)
{
    Serial.println(" Handle2  ");
}

void handle_irq3(void)
{
    Serial.println(" Handle3  ");
}

void setup() {

    attachInterrupt(D6, handle_irq1, RISING);
    attachInterrupt(D7, handle_irq2, FALLING);
    attachInterrupt(D8, handle_irq3, CHANGE);

    pinMode(D13, OUTPUT);
    Serial.begin(9600);
    Serial.println("External Interrupter Demo ");
}

void loop() {
    // put your main code here, to run repeatedly:
    digitalWrite(D13,HIGH);
    delay(1000);
    digitalWrite(D13, LOW);
    delay(1000);
}


