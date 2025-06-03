/*
MIT License

Copyright (c) 2025 Nguyen Hoang Minh Triet

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

-----------------------------------------------------------------------
Author : Nguyen Hoang Minh Triet 
Email  : 23trietminh23@gmail.com  
GitHub : https://github.com/nguyentriet0810  
YouTube: https://www.youtube.com/@hoangtriet9999
-----------------------------------------------------------------------
*/

#ifndef __OS__
#define __OS__

#include <stm32f10x.h>
#include <stdint.h>

#include "timer.h"
#include "step_driver.h"

/* Define maximum number of threads supported by the OS */
#define NUM_OF_THREADS     7U

/* Define stack size (in 32-bit words) for each thread */
#define STACK_SIZE         400U

/* System clock frequency in Hz */
#define BUS_FREQ           72000000UL

/* SysTick interrupt control register */
#define INTCTRL            (*((volatile uint32_t *)0xE000ED04U))

/* SysTick pending interrupt bit (bit 26) */
#define PENDSTSET          (1UL << 26U)

/* Buffer size for USART1 communication */
#define USART1_BUFFER_SIZE 16U

/* -------------------------------------------------------------------------- */
/* VCU Receive Frame Structure (16 bytes total)
 *
 * Byte  Offset | Description     | Type     | Size
 * ------------ | --------------- | -------- | -----
 *      0       | Command         | uint8_t  | 1
 *      1       | Pull mode       | uint8_t  | 1
 *      2       | Res mode        | uint8_t  | 1
 *      3       | Force           | uint32_t | 4
 *      7       | Times           | uint32_t | 4
 *     11       | Balance         | uint8_t  | 1
 *     12       | Speed           | uint32_t | 4
 *
 * Total: 16 bytes
 */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* VCU Transmit Frame Structure (27 bytes total)
 *
 * Byte  Offset | Description     | Type     | Size
 * ------------ | --------------- | -------- | -----
 *      0       | Command         | uint8_t  | 1
 *      1       | Force           | int32_t  | 5 
 *      6       | Resistor        | uint32_t | 10 
 *     16       | Length          | uint8_t  | 5
 *     21       | Times           | uint8_t  | 5
 *     26       | Mode            | uint8_t  | 1
 *
 * Total: 27 bytes
 *
 * NOTE: Field sizes and names should be confirmed against protocol specification.
 */
/* -------------------------------------------------------------------------- */

/* Thread Control Block structure definition */
struct tcb
{
    uint32_t *stackPt;     /* Pointer to top of task stack */
    struct tcb *nextPt;    /* Pointer to next task in scheduling list */
    uint32_t Sleep;        /* Sleep duration counter (ticks) */
};

/* Create a type alias for the thread control block */
typedef struct tcb tcbType;

/* Global declarations of thread management structures */
extern tcbType tcbs[NUM_OF_THREADS];              /* Array of TCBs */
extern uint32_t current_num_of_thread;            /* Current number of active threads */
extern uint8_t buffer_usart1_data[];              /* USART1 receive buffer */
extern uint8_t Sema_usart1_handler;               /* Semaphore for USART1 handler */

/* Initialize kernel structures and state */
void osKernelInit(void);

/* Launch the OS scheduler with specified time slice (quanta) */
void osKernelLaunch(uint8_t quanta);

/* Assembly function to launch the first thread */
void osSchedulerLaunch(void);

/* Initialize stack for thread 'i' */
void osKernelStackInit(int i);

/* Add one new thread to the OS and return success status */
uint8_t osKernelAdd1Thread(void (*task)(void));

/* Perform round-robin thread scheduling */
void osSchedulerRoundRobin(void);

/* Yield CPU to next ready thread */
void osThreadYield(void);

/* Initialize a semaphore with given initial value */
void osSemaphore_Init(uint8_t *semaphore, uint8_t val);

/* Release (give) a semaphore */
void osSemaphore_Give(uint8_t *semaphore);

/* Wait (block) until semaphore is available (cooperative multitasking) */
void osCooperative_Wait(uint8_t *semaphore);

/* Put the calling thread to sleep for specified time (in ticks) */
void osThreadsSleep(uint32_t sleeptime);

#endif /* __OS__ */
