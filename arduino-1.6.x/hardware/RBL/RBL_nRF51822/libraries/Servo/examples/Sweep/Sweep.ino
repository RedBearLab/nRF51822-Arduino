// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.


#include <Servo.h> 
 
Servo myservo[4];  

int pos = 0;   
 
void setup() 
{ 
    myservo[0].attach(D2);  
    myservo[1].attach(D3);  
    myservo[2].attach(D4); 
    myservo[3].attach(D5);  
} 
 
 
void loop() 
{ 
    for(pos = 0; pos < 180; pos += 1)  
    {                                  
        myservo[0].write(pos);          
        myservo[1].write(pos);      
        myservo[2].write(pos);          
        myservo[3].write(pos);           
        delay(15);                      
    } 
    for(pos = 180; pos >= 1; pos -= 1) 
    {                                
        myservo[0].write(pos);          
        myservo[1].write(pos);          
        myservo[2].write(pos);          
        myservo[3].write(pos);          
        delay(15);                       
     } 
} 
