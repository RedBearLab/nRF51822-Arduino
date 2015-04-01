#include "nrf_err_check.h"
#include "Arduino.h"

#define NRF_ERR_CHECK  1

void nrf_err_check(unsigned int err_code)
{
    if(err_code != 0)
    {
#ifdef NRF_ERR_CHECK
        Serial.print("err_code = ");
        Serial.println(err_code, HEX);
#endif    
        while(1);
    }
}























