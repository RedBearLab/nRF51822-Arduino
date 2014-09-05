#ifndef __DEBUG_H_
#define __DEBUG_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

/**
 * @defgroup debug_logger Debug Logger
 * @ingroup experimental_api
 * @{
 * @brief Enables debug logs/ trace over UART.
 * @details Enables debug logs/ trace over UART. Tracing is enabled only if 
 *          ENABLE_DEBUG_LOG_SUPPORT is defined in the project.
 */
#ifdef ENABLE_DEBUG_LOG_SUPPORT
/**
 * @brief Module Initialization.
 *
 * @details Initializes the module.
 */
void debug_init(void);

/**
 * @brief Log debug messages.
 *
 * @details This API logs messages over UART. Module shall be initialized before using this API.
 *
 * @note Though this is currently a macro, it should be used used and treated as function.
 */
#define debug_log printf
void debug_dump(uint8_t * str, uint32_t len);
#else // ENABLE_DEBUG_LOG_SUPPORT
#define debug_init(...)
#define debug_log(...)
#define debug_dump(...)
#endif // ENABLE_DEBUG_LOG_SUPPORT

/** @} */

#ifdef __cplusplus
}
#endif
#endif //__DEBUG_H_
