/*
 * Copyright (c) Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 *   3. Neither the name of Nordic Semiconductor ASA nor the names of other
 *   contributors to this software may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef NRF_GPIOTE_H__
#define NRF_GPIOTE_H__

#include "nrf.h"

/**
* @defgroup nrf_gpiote GPIOTE abstraction
* @{
* @ingroup nrf_drivers
* @brief GPIOTE abstraction for configuration of channels.
*/


 /**
 * @enum nrf_gpiote_polarity_t
 * @brief Polarity for GPIOTE channel enumerator.
 */
typedef enum
{
  NRF_GPIOTE_POLARITY_LOTOHI = GPIOTE_CONFIG_POLARITY_LoToHi,       ///<  Low to high
  NRF_GPIOTE_POLARITY_HITOLO = GPIOTE_CONFIG_POLARITY_HiToLo,       ///<  High to low
  NRF_GPIOTE_POLARITY_TOGGLE = GPIOTE_CONFIG_POLARITY_Toggle        ///<  Toggle
} nrf_gpiote_polarity_t;


 /**
 * @enum nrf_gpiote_outinit_t
 * @brief Initial output value for GPIOTE channel enumerator.
 */
typedef enum
{
  NRF_GPIOTE_INITIAL_VALUE_LOW  = GPIOTE_CONFIG_OUTINIT_Low,       ///<  Low to high
  NRF_GPIOTE_INITIAL_VALUE_HIGH = GPIOTE_CONFIG_OUTINIT_High       ///<  High to low
} nrf_gpiote_outinit_t;


/**
 * @brief Function for configuring GPIOTE channel as output, setting the properly desired output level.
 *
 *
 * @param channel_number specifies the GPIOTE channel [0:3] to configure as an output channel.
 * @param pin_number specifies the pin number [0:30] to use in the GPIOTE channel.
 * @param polarity specifies the desired polarity in the output GPIOTE channel.
 * @param initial_value specifies the initial value of the GPIOTE channel input after the channel configuration.
 */
static __INLINE void nrf_gpiote_task_config(uint32_t channel_number, uint32_t pin_number, nrf_gpiote_polarity_t polarity, nrf_gpiote_outinit_t initial_value)
{
    /* Check if the output desired is high or low */
    if (initial_value == NRF_GPIOTE_INITIAL_VALUE_LOW)
    {
        /* Workaround for the OUTINIT PAN. When nrf_gpiote_task_config() is called a glitch happens
        on the GPIO if the GPIO in question is already assigned to GPIOTE and the pin is in the 
        correct state in GPIOTE but not in the OUT register. */
        NRF_GPIO->OUTCLR = (1 << pin_number);
        
        /* Configure channel to Pin31, not connected to the pin, and configure as a tasks that will set it to proper level */
        NRF_GPIOTE->CONFIG[channel_number] = (GPIOTE_CONFIG_MODE_Task       << GPIOTE_CONFIG_MODE_Pos)     |
                                             (31UL                          << GPIOTE_CONFIG_PSEL_Pos)     |
                                             (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos);                                    
    } 
    else 
    {
        /* Workaround for the OUTINIT PAN. When nrf_gpiote_task_config() is called a glitch happens
        on the GPIO if the GPIO in question is already assigned to GPIOTE and the pin is in the 
        correct state in GPIOTE but not in the OUT register. */
        NRF_GPIO->OUTSET = (1 << pin_number);
        
        /* Configure channel to Pin31, not connected to the pin, and configure as a tasks that will set it to proper level */
        NRF_GPIOTE->CONFIG[channel_number] = (GPIOTE_CONFIG_MODE_Task       << GPIOTE_CONFIG_MODE_Pos)     |
                                             (31UL                          << GPIOTE_CONFIG_PSEL_Pos)     |
                                             (GPIOTE_CONFIG_POLARITY_LoToHi << GPIOTE_CONFIG_POLARITY_Pos);
    }

    /* Three NOPs are required to make sure configuration is written before setting tasks or getting events */
    __NOP();
    __NOP();
    __NOP(); 

    /* Launch the task to take the GPIOTE channel output to the desired level */
    NRF_GPIOTE->TASKS_OUT[channel_number] = 1;
    

    /* Finally configure the channel as the caller expects. If OUTINIT works, the channel is configured properly. 
       If it does not, the channel output inheritance sets the proper level. */
    NRF_GPIOTE->CONFIG[channel_number] = (GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos)     |
                                         ((uint32_t)pin_number    << GPIOTE_CONFIG_PSEL_Pos)     |
                                         ((uint32_t)polarity      << GPIOTE_CONFIG_POLARITY_Pos) |
                                         ((uint32_t)initial_value << GPIOTE_CONFIG_OUTINIT_Pos);

    /* Three NOPs are required to make sure configuration is written before setting tasks or getting events */
    __NOP();
    __NOP();
    __NOP(); 
}

/**
 * @brief Function for configuring GPIOTE channel as input, automatically clearing an event that appears in some cases under configuration.
 *
 * Note that when configuring the channel as input an event might be triggered. Care of disabling interrupts
 * for that channel is left to the user.
 *
 * @param channel_number specifies the GPIOTE channel [0:3] to configure as an input channel.
 * @param pin_number specifies the pin number [0:30] to use in the GPIOTE channel.
 * @param polarity specifies the desired polarity in the output GPIOTE channel.
 */
static __INLINE void nrf_gpiote_event_config(uint32_t channel_number, uint32_t pin_number, nrf_gpiote_polarity_t polarity)
{   
    /* Configure the channel as the caller expects */
    NRF_GPIOTE->CONFIG[channel_number] = (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos)     |
                                         ((uint32_t)pin_number     << GPIOTE_CONFIG_PSEL_Pos)     |
                                         ((uint32_t)polarity       << GPIOTE_CONFIG_POLARITY_Pos);

    /* Three NOPs are required to make sure configuration is written before setting tasks or getting events */
    __NOP();
    __NOP();
    __NOP();
    
    /* Clear the event that appears in some cases */
    NRF_GPIOTE->EVENTS_IN[channel_number] = 0; 
}


/**
 * @brief Function for unconfiguring GPIOTE channel.
 *
 *
 * Note that when unconfiguring the channel, the pin is configured as GPIO PIN_CNF configuration.
 *
 * @param channel_number specifies the GPIOTE channel [0:3] to unconfigure.
 */
static __INLINE void nrf_gpiote_unconfig(uint32_t channel_number)
{   
    /* Unonfigure the channel as the caller expects */
    NRF_GPIOTE->CONFIG[channel_number] = (GPIOTE_CONFIG_MODE_Disabled   << GPIOTE_CONFIG_MODE_Pos) |
                                         (31UL                          << GPIOTE_CONFIG_PSEL_Pos) |
                                         (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos);
}


/** @} */

#endif
