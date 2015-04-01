

#include "app_error_check.h"

 
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    NVIC_SystemReset();
}


