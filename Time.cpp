#include "Time.h"

#if defined(USE_TIMER0)

/*********************************************
Function: __Time__()
Purpose:  Constructor to __Time__ class
Input:    Timer register
Return:   None
*********************************************/
__Time__::__Time__(volatile uint8_t* tccra, volatile uint8_t* tccrb,\
                   volatile uint8_t* timsk, volatile uint8_t* ocra)
{
    this->tccra = tccra; this->tccrb = tccrb;
    this->timsk = timsk; this->ocra = ocra;
}

/*********************************************
Function: __Time__()
Purpose:  Destructor to __Time__ class
Input:    None
Return:   None
*********************************************/
__Time__::~__Time__()
{
    /* Empty */
}

/*********************************************
Function: begin()
Purpose:  Begin time implementation
Input:    None
Return:   None
*********************************************/
void __Time__::begin(void)
{
    if (this->isOn) return; /* If time implementation is already started */

     ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        this->isOn = 1;                               /* Set a flag that timer is on */
        *this->tccra |= (1 << WGM01);                 /* Wave Generation Mode is set to Clear Timer Output Compare (CTC) */
        *this->tccrb |= (1 << CS01);                  /* Select clock (prescaler to 8) */
        *this->timsk |= (1 << OCIE0A);                /* Enable Output Compare Match Interrupt */
        *this->ocra = ROUND((F_CPU / 80000.0) - 1.0); /* Calculate Value To Overflow Every 100 uS */
    }
}

/*********************************************
Function: delayMillis()
Purpose:  Delay a specified amount of time in milliseconds
Input:    Amount of time to be delayed
Return:   None
*********************************************/
void __Time__::delayMillis(time_t time)
{
    time_t timestamp = this->millis();
    while ((this->millis() - timestamp) < time);
}

/*********************************************
Function: millis()
Purpose:  Get value of milliseconds passed since Timer1 started counting
Input:    None
Return:   Value of milliseconds
*********************************************/
time_t __Time__::millis(void)
{
    return (this->micros() / 1000);
}

/*********************************************
Function: micros()
Purpose:  Get value of microseconds passed since Timer1 started counting
Input:    None
Return:   Value of microseconds
*********************************************/
time_t __Time__::micros(void)
{
    time_t time;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        time = this->counter;
    }
    return (time);
}

/*********************************************
Function: end()
Purpose:  End time implementation
Input:    None
Return:   None
*********************************************/
void __Time__::end(void)
{
    #if defined (__AVR_ATmega328P__)
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            this->counter = 0; /* Clear counter */
            this->isOn = 0;    /* Clear a flag that timer is on */
            *this->tccra = 0;  /* Wave Generation Mode is set to Clear Timer Output Compare (CTC) */
            *this->tccrb = 0;  /* Select clock (prescaler to 8) */
            *this->timsk = 0;  /* Enable Output Compare Match Interrupt */
            *this->ocra = 0;   /* Calculate Value To Overflow Every 100 uS */
        }
    #endif
}

inline void __Time__::countIRQ(void)
{
    this->counter += MICROS_RESOLUTION;
}

#endif

#if defined(USE_TIMER0)
__Time__ Time = __Time__(&TCCR0A, &TCCR0B, &TIMSK0, &OCR0A);
#endif

/*********************************************
Function: ISR()
Purpose:  Interrupt Service Routine for incrementing variable
Input:    Vector
Return:   None
*********************************************/
#if defined(USE_TIMER0)
ISR(TIMER0_COMPA_vect)
#endif
{
    Time.countIRQ();
}
