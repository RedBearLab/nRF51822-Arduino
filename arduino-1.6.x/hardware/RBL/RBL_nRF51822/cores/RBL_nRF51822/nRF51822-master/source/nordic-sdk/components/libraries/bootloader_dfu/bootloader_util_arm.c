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
#include "compiler_abstraction.h"
#include "bootloader_util.h"
#include <stdint.h>
#include <string.h>

/**
 * @brief Function for aborting current application/bootloader jump to to other app/bootloader.
 *
 * @details This functions will use the address provide to swap the stack pointer and then load
 *          the address of the reset handler to be executed. It will check current system mode
 *          (thread/handler) and if in thread mode it will reset into other application.
 *          If in handler mode \ref isr_abort will be executed to ensure correct exit of handler
 *          mode and jump into reset handler of other application.
 *
 * @param[in]  start_addr  Start address of other application. This address must point to the
               initial stack pointer of the application.
 *
 * @note This function will never return but issue a reset into provided application.
 */
#ifdef TOOLCHAIN_ARM
__asm static void bootloader_util_reset(uint32_t start_addr)
{
MASK_ONES       EQU 0xFFFFFFFF  ; Ones, to be loaded into register as default value before reset.
MASK_ZEROS      EQU 0x00000000  ; Zeros, to be loaded into register as default value before reset.
xPSR_RESET      EQU 0x21000000  ; Default value of xPSR after System Reset.
EXC_RETURN_CMD  EQU 0xFFFFFFF9  ; EXC_RETURN for ARM Cortex. When loaded to PC the current interrupt service routine (handler mode) willl exit and the stack will be popped. Execution will continue in thread mode.
    LDR   R5, [R0]              ; Get App initial MSP for bootloader.
    MSR   MSP, R5               ; Set the main stack pointer to the applications MSP.
    LDR   R6, [R0, #0x04]       ; Load Reset handler into register 6.

    LDR   R2, =MASK_ZEROS       ; Load zeros to R2
    MRS   R3, IPSR              ; Load IPSR to R3 to check for handler or thread mode
    CMP   R2, R3                ; Compare, if 0 then we are in thread mode and can continue to reset handler of bootloader
    MOV   R0, R6
    BNE   isr_abort             ; If not zero we need to exit current ISR and jump to reset handler of bootloader

    LDR   R4, =MASK_ONES        ; Load ones to R4 to be placed in Link Register.
    MOV   LR, R4                ; Clear the link register and set to ones to ensure no return.
    BX    R6                    ; Branch to reset handler of bootloader
isr_abort
    LDR   R4,=MASK_ONES         ; Fill with ones before jumping to reset handling. We be popped as R12 when exiting ISR (Cleaning up the registers).
    LDR   R5,=MASK_ONES         ; Fill with ones before jumping to reset handling. We be popped as LR when exiting ISR. Ensures no return to application.
    MOV   R6, R0                ; Move address of reset handler to R6. Will be popped as PC when exiting ISR. Ensures the reset handler will be executed when exist ISR.
    LDR   R7,=xPSR_RESET        ; Move reset value of xPSR to R7. Will be popped as xPSR when exiting ISR.
    PUSH  {r4-r7}               ; Push everything to new stack to allow interrupt handler to fetch it on exiting the ISR.

    LDR   R4,=MASK_ZEROS        ; Fill with zeros before jumping to reset handling. We be popped as R0 when exiting ISR (Cleaning up of the registers).
    LDR   R5,=MASK_ZEROS        ; Fill with zeros before jumping to reset handling. We be popped as R1 when exiting ISR (Cleaning up of the registers).
    LDR   R6,=MASK_ZEROS        ; Fill with zeros before jumping to reset handling. We be popped as R2 when exiting ISR (Cleaning up of the registers).
    LDR   R7,=MASK_ZEROS        ; Fill with zeros before jumping to reset handling. We be popped as R3 when exiting ISR (Cleaning up of the registers).
    PUSH  {r4-r7}               ; Push zeros (R4-R7) to stack to prepare for exiting the interrupt routine.

    LDR   R0,=EXC_RETURN_CMD    ; Load the execution return command into register.
    BX    R0                    ; No return - Handler mode will be exited. Stack will be popped and execution will continue in reset handler initializing other application.
    ALIGN
}
#elif defined(TOOLCHAIN_GCC)
static void bootloader_util_reset(uint32_t start_addr)
{
    __ASM(
        ".equ MASK_ONES,  0xFFFFFFFF\n\t" /* Ones, to be loaded into register as default value before reset.  */
        ".equ MASK_ZEROS, 0x00000000\n\t" /* Zeros, to be loaded into register as default value before reset. */
        ".equ xPSR_RESET, 0x21000000\n\t" /* Default value of xPSR after System Reset. */
        ".equ EXC_RETURN_CMD, 0xFFFFFFF9\n\t" /* EXC_RETURN for ARM Cortex. When loaded to PC the current interrupt service routine (handler mode) willl exit and the stack will be popped. Execution will continue in thread mode. */

        "LDR   r5, [r0]       \n\t"       /* Get App initial MSP for bootloader.                              */
        "MSR   MSP, r5        \n\t"       /* Set the main stack pointer to the applications MSP.              */
        "LDR   r6,[r0, #0x04] \n\t"       /* Load Reset handler into register 0.                              */

        "LDR   r2, =MASK_ZEROS\n\t"       /* Load zeros to R2                                                 */
        "MRS   r3, IPSR       \n\t"       /* Load IPSR to R3 to check for handler or thread mode              */
        "CMP   r2, r3         \n\t"       /* Compare, if 0 then we are in thread mode and can continue to reset handler of bootloader */
        "MOV   R0, R6         \n\t"
        "BNE   isr_abort      \n\t"       /* If not zero we need to exit current ISR and jump to reset handler of bootloader */

        "LDR   r4, =MASK_ONES \n\t"       /* Load ones to R4 to be placed in Link Register.                   */
        "MOV   LR, r4         \n\t"       /* Clear the link register and set to ones to ensure no return.     */
        "BX    r6             \n\t"       /* Branch to reset handler of bootloader                            */
        "isr_abort:           \n\t"
        "LDR   R4,=MASK_ONES \n\t" /* Fill with ones before jumping to reset handling. We be popped as R12 when exiting ISR (Cleaning up the registers). */
        "LDR   R5,=MASK_ONES \n\t" /* Fill with ones before jumping to reset handling. We be popped as LR when exiting ISR. Ensures no return to application. */
        "MOV   R6, R0        \n\t" /* Move address of reset handler to R6. Will be popped as PC when exiting ISR. Ensures the reset handler will be executed when exist ISR. */
        "LDR   R7,=xPSR_RESET\n\t" /* Move reset value of xPSR to R7. Will be popped as xPSR when exiting ISR. */
        "PUSH  {r4-r7}       \n\t" /* Push everything to new stack to allow interrupt handler to fetch it on exiting the ISR. */

        "LDR   R4,=MASK_ZEROS\n\t" /* Fill with zeros before jumping to reset handling. We be popped as R0 when exiting ISR (Cleaning up of the registers). */
        "LDR   R5,=MASK_ZEROS\n\t" /* Fill with zeros before jumping to reset handling. We be popped as R1 when exiting ISR (Cleaning up of the registers). */
        "LDR   R6,=MASK_ZEROS\n\t" /* Fill with zeros before jumping to reset handling. We be popped as R2 when exiting ISR (Cleaning up of the registers). */
        "LDR   R7,=MASK_ZEROS\n\t" /* Fill with zeros before jumping to reset handling. We be popped as R3 when exiting ISR (Cleaning up of the registers). */
        "PUSH  {r4-r7}       \n\t" /* Push zeros (R4-R7) to stack to prepare for exiting the interrupt routine. */

        "LDR   R0,=EXC_RETURN_CMD\n\t" /* Load the execution return command into register. */
        "BX    R0                \n\t" /* No return - Handler mode will be exited. Stack will be popped and execution will continue in reset handler initializing other application. */
        : /* output operands */
        : /* input operands */
        : "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7" /* clobber list */
        );
}
#endif /* TOOLCHAIN_ARM */

void bootloader_util_app_start(uint32_t start_addr)
{
    bootloader_util_reset(start_addr);
}
