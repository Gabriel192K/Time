#ifndef __TIME_H__
#define __TIME_H__

/* Dependecies */
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>        /* Memory */
#include <avr/interrupt.h> /* Interrupts */
#include <util/atomic.h>   /* Atomic Operaions */

/* Macros */
#define TIMER_RANGE          (uint16_t)256 /* How many different values */
#define TIMER_PRESCALER      (uint8_t)8
#define TIME_INCREMENT_VALUE (uint32_t)((TIMER_PRESCALER * TIMER_RANGE) / (F_CPU / 1e6)) /* In microseconds */
#define DELAY_US_CONSTANT    (F_CPU / 4e6)

class __Time__
{
    public:
        __Time__();
        ~__Time__();
        void     begin       (void);
        void     reset       (void);
        void     delay       (uint32_t s);
        void     delayMillis (uint32_t ms);
        uint32_t seconds     (void);
        uint32_t milliseconds(void);
        uint32_t microseconds(void);
        void     end         (void);
        void     irq         (void);
    private:
        uint8_t beginFunctionCalled;
        volatile uint32_t counter;
};
extern __Time__ Time;

/*********************************************
Function: delayMicros()
Purpose:  Delay a specified amount of time in microseconds
Input:    Amount of microseconds to be delayed
Return:   None
*********************************************/
static inline __attribute__((always_inline)) void delayMicros(uint32_t us)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        uint32_t ticks = us * DELAY_US_CONSTANT;
        __asm__ __volatile__
        (
            "1: sbiw %0,1" "\n\t"
            "brne 1b"
            : "=w" (ticks)
            : "0" (ticks)
        );
    }
}

#endif
