#ifndef __TIME_H__
#define __TIME_H__

/* Dependecies */
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>        /* Memory */
#include <avr/interrupt.h> /* Interrupts */
#include <util/atomic.h>   /* Atomic Operaions */

/**
 * @brief  The range of the timer, representing how many different values it can take.
 * @note   This value is the maximum count the timer will go through.
 */
#define TIMER_RANGE (const uint16_t)256

/**
 * @brief  The prescaler for the timer.
 * @note   This value is used to divide the system clock to get the timer's actual frequency.
 */
#define TIMER_PRESCALER (const uint8_t)8

/**
 * @brief  The time increment value in microseconds.
 * @note   This represents the time that passes for each timer tick, calculated based on the 
 *         prescaler, timer range, and the system clock frequency (F_CPU).
 */
#define TIME_INCREMENT_VALUE (const uint32_t)((TIMER_PRESCALER * TIMER_RANGE) / (F_CPU / 1e6))

/**
 * @brief  The constant used to calculate time delay in microseconds.
 * @note   This value is used for delay calculation based on the system clock frequency.
 */
#define DELAY_US_CONSTANT (F_CPU / 4e6)


/**
 * @class __Time__
 * @brief Class for managing and tracking time using Timer 0.
 * @details This class provides methods to manage and track time in terms of microseconds, 
 *          milliseconds, and seconds using Timer 0. It supports operations such as 
 *          initialization, time delays, and interrupt-based updates to maintain accurate time.
 * 
 * The class interacts directly with the hardware registers for Timer 0 (`TCCR0B` and `TIMSK0`),
 * and provides an interface to initialize the timer, reset the counter, create delays, 
 * and retrieve the elapsed time.
 */
class __Time__
{
    public:
        /**
         * @brief Constructor to initialize Timer 0 with given registers.
         * @param tccrb Pointer to the TCCR0B register for Timer 0 control.
         * @param timsk Pointer to the TIMSK0 register for Timer 0 interrupt mask.
         */
        __Time__(volatile uint8_t* tccrb, volatile uint8_t* timsk);

        /**
         * @brief Destructor to clean up Timer 0 settings.
         */
        ~__Time__();

        /**
         * @brief Initializes Timer 0 with a prescaler and interrupt.
         * @return 1 if initialization is successful, 0 if already initialized.
         */
        const uint8_t begin(void);

        /**
         * @brief Resets the time counter to 0.
         */
        void reset(void);

        /**
         * @brief Creates a delay in seconds.
         * @param s The number of seconds to delay.
         */
        void delay(const uint32_t s);

        /**
         * @brief Creates a delay in milliseconds.
         * @param ms The number of milliseconds to delay.
         */
        void delayMillis(const uint32_t ms);

        /**
         * @brief Returns the elapsed time in seconds.
         * @return The number of seconds since the timer started.
         */
        const uint32_t seconds(void);

        /**
         * @brief Returns the elapsed time in milliseconds.
         * @return The number of milliseconds since the timer started.
         */
        const uint32_t milliseconds(void);

        /**
         * @brief Returns the elapsed time in microseconds.
         * @return The number of microseconds since the timer started.
         */
        const uint32_t microseconds(void);

        /**
         * @brief Stops Timer 0 and disables the interrupt.
         * @return 1 if successful, 0 if Timer 0 was not started.
         */
        const uint8_t end(void);

        /**
         * @brief Timer overflow interrupt handler to update the time counter.
         * @details This method is called by the ISR to increment the time counter 
         *          by a predefined increment value.
         */
        void isr(void);

    private:
        volatile uint8_t* tccrb;  /**< Pointer to the TCCR0B register for Timer 0 control */
        volatile uint8_t* timsk;  /**< Pointer to the TIMSK0 register for Timer 0 interrupt mask */
        uint8_t began;            /**< Flag indicating whether the timer has been initialized */
        volatile uint32_t counter; /**< The current time counter, tracking microseconds */
};


#if defined(__AVR_ATmega328__) || \
    defined(__AVR_ATmega328P__) || \
    defined(__AVR_ATmega328PB__) || \
    defined(__AVR_ATmega2560__)
extern __Time__ Time;
#endif

#if defined(__AVR_ATmega328__) || \
    defined(__AVR_ATmega328P__) || \
    defined(__AVR_ATmega328PB__) || \
    defined(__AVR_ATmega2560__)
/**
 * @brief Delays execution for a specified number of microseconds.
 * @param us The number of microseconds to delay.
 * @details This function provides a precise delay for the given number of microseconds 
 *          by utilizing an inline assembly loop. It uses an atomic block to prevent 
 *          interruptions during the delay period.
 * 
 * @note This function is designed for use with AVR-based microcontrollers, specifically 
 *       the ATmega328, ATmega328P, ATmega328PB, and ATmega2560. The delay is calculated 
 *       by decrementing a counter value derived from the input time in microseconds.
 */
static inline __attribute__((always_inline)) void delayMicros(const uint32_t us)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        uint32_t ticks = us * DELAY_US_CONSTANT;  /**< Calculate the number of ticks corresponding to the microseconds */
        __asm__ __volatile__
        (
            "1: sbiw %0,1" "\n\t"                    /**< Decrement the tick counter by 1 */
            "brne 1b"                                /**< Repeat the loop until the counter reaches zero */
            : "=w" (ticks)
            : "0" (ticks)
        );
    }
}

#endif
#endif
