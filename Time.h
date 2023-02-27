#ifndef __TIME_H__
#define __TIME_H__

/* Dependecies */
#include "Core\Core.h"
#include "Utilities\Interrupts.h"
#include "Utilities\Math.h"

/* Macros */
#define MICROS_RESOLUTION 100

/* Variables */
typedef uint32_t time_t;

class __Time__
{
    public:
        __Time__(volatile uint8_t* tccra, volatile uint8_t* tccrb,\
                 volatile uint8_t* timsk, volatile uint8_t* ocra);
        ~__Time__();
        void   begin      (void);
        void   delayMillis(time_t time);
        time_t millis     (void);
        time_t micros     (void);
        void   end        (void);
        inline void countIRQ(void);
    private:
        volatile time_t counter;
        uint8_t isOn;

        volatile uint8_t* tccra;
        volatile uint8_t* tccrb;
        volatile uint8_t* timsk;
        volatile uint8_t* ocra;
};

#if defined(__AVR_ATmega328P__)
#define USE_TIMER0
extern __Time__ Time;
#endif

#endif
