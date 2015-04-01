#ifndef _COMMON_H
#define _COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "projectconfig.h"
#include "compiler.h"
#include "ble_error.h"
#include "simple_uart.h"
#include "nrf_err_check.h"

#include <string.h>
#include "nrf_gpio.h"


#define ASSERT_STATUS_MESSAGE(sts, message)
#define ASSERT_STATUS(sts)                     ( (sts==0) ? nrf_gpio_pin_clear(19) : nrf_gpio_pin_clear(18) )
#define ASSERT_STATUS_RET_VOID(sts)
//--------------------------------------------------------------------+
// Logical Assert
//--------------------------------------------------------------------+
#define ASSERT(...)                       
#define ASSERT_TRUE(condition  , error)  
#define ASSERT_FALSE(condition , error)       

#define ASSERT_INT(...)                     
#define ASSERT_INT_EQUAL(...)           
#define ASSERT_INT_WITHIN(...)             

//--------------------------------------------------------------------+
// INLINE FUNCTION
//--------------------------------------------------------------------+
#define memclr_(buffer, size)  memset(buffer, 0, size)

#ifdef __cplusplus
}
#endif

#endif
