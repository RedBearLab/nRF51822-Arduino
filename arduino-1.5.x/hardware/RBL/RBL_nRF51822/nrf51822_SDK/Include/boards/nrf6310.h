/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#ifndef NRF6310_H__
#define NRF6310_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "nrf_gpio.h"

#define LED_START      8
#define LED_0          8
#define LED_1          9
#define LED_2          10
#define LED_3          11
#define LED_4          12
#define LED_5          13
#define LED_6          14
#define LED_7          15
#define LED_STOP       15

#define BUTTON_START   0
#define BUTTON_0       0
#define BUTTON_1       1
#define BUTTON_2       2
#define BUTTON_3       3
#define BUTTON_4       4
#define BUTTON_5       5
#define BUTTON_6       6
#define BUTTON_7       7
#define BUTTON_STOP    7
#define BUTTON_PULL    NRF_GPIO_PIN_NOPULL

#define RX_PIN_NUMBER  16    // UART RX pin number.
#define TX_PIN_NUMBER  17    // UART TX pin number.
#define CTS_PIN_NUMBER 18    // UART Clear To Send pin number. Not used if HWFC is set to false
#define RTS_PIN_NUMBER 19    // Not used if HWFC is set to false
#define HWFC           false // UART hardware flow control

#define SPIS_MISO_PIN  20    // SPI MISO signal. 
#define SPIS_CSN_PIN   21    // SPI CSN signal. 
#define SPIS_MOSI_PIN  22    // SPI MOSI signal. 
#define SPIS_SCK_PIN   23    // SPI SCK signal. 
#ifdef __cplusplus
}
#endif
#endif  // NRF6310_H__
