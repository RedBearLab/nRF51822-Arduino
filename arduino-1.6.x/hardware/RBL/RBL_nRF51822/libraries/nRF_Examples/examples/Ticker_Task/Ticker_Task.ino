/*
    This example show how to use timer task.
*/

Ticker ticker1, ticker2;

void m_ticker1_handle(void)
{
    Serial1.println("handle1 ");    
}

void m_ticker2_handle(void)
{
    Serial1.println("handle2 ");    
}

void setup() {
    // put your setup code here, to run once:
    ticker1.attach_us(m_ticker1_handle, 100000);
    ticker2.attach_us(m_ticker2_handle, 200000);

    pinMode(D13, OUTPUT);
    Serial1.begin(115200);
    Serial1.println("Ticker Task Test ");    
}

void loop() {
    // put your main code here, to run repeatedly:
    digitalWrite(D13,HIGH);
    delay(500);
    digitalWrite(D13, LOW);
    delay(500);
}
