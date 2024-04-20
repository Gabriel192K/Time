#include "Time.h"

/*********************************************
Function: __Time__()
Purpose:  Constructor to __Time__ class
Input:    None
Return:   None
*********************************************/
__Time__::__Time__()
{
    /* Empty */
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
Return:   Status of begin
*********************************************/
const uint8_t __Time__::begin(void)
{
    /* If time implementation is already started */
    if (this->beginCalled)
        return (0);
    this->beginCalled = 1;
    
    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
    #if defined(__AVR_ATmega328P__) || \
        defined(__AVR_ATmega328PB__) || \
        defined(__AVR_ATmega2560__)
        /* Select clock prescaler to 8 */
        TCCR0B = TCCR0B & ~((1 << CS02) | (1 << CS00));
        TCCR0B = TCCR0B | (1 << CS01);
        /* Enable Timer Overflow Interrupt */
        TIMSK0 = TIMSK0 | (1 << TOIE0);
    #else
        #error "MCU DOES NOT SUPPORT TIME IMPLEMENTATION"
    #endif
    }
    return (1);
}

void __Time__::reset(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        /* Reset variable */
        this->counter = 0;
    }
}

/*********************************************
Function: delay()
Purpose:  Delay a specified amount of time in seconds
Input:    Amount of seconds to be delayed
Return:   None
*********************************************/
void __Time__::delay(uint32_t s)
{
    uint32_t timestamp = this->seconds();
    while ((this->seconds() - timestamp) < s);
}

/*********************************************
Function: delayMillis()
Purpose:  Delay a specified amount of time in milliseconds
Input:    Amount of milliseconds to be delayed
Return:   None
*********************************************/
void __Time__::delayMillis(uint32_t ms)
{
    uint32_t timestamp = this->milliseconds();
    while ((this->milliseconds() - timestamp) < ms);
}

/*********************************************
Function: seconds()
Purpose:  Get value of seconds passed since Timer started counting
Input:    None
Return:   Value of seconds
*********************************************/
const uint32_t __Time__::seconds(void)
{
    return (this->milliseconds() / 1000);
}

/*********************************************
Function: milliseconds()
Purpose:  Get value of milliseconds passed since Timer started counting
Input:    None
Return:   Value of milliseconds
*********************************************/
const uint32_t __Time__::milliseconds(void)
{
    return (this->microseconds() / 1000);
}

/*********************************************
Function: microseconds()
Purpose:  Get value of microseconds passed since Timer started counting
Input:    None
Return:   Value of microseconds
*********************************************/
const uint32_t __Time__::microseconds(void)
{
    uint32_t time;
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
const uint8_t __Time__::end(void)
{
    if (!this->beginCalled)
        return (0);
    this->beginCalled = 0;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        #if defined(__AVR_ATmega328P__) || \
            defined(__AVR_ATmega328PB__) || \
            defined(__AVR_ATmega2560__)
        /* Reset Timer Overflow Interrupt */
        TIMSK0 = TIMSK0 & ~(1 << TOIE0);
        #else
            #error "MCU DOES NOT SUPPORT TIME IMPLEMENTATION"
        #endif
        /* Clear counter */
        this->counter = 0; 
    }
	return (1);
}

void __Time__::irq(void)
{
    this->counter = (this->counter + TIME_INCREMENT_VALUE);
}

__Time__ Time = __Time__();

/************************
Function: Interrupt Service Routine
Purpose:  Handling interrupts of Timer COMPA
Input:    Interrupt vector
Return:   None
************************/
#if defined(__AVR_ATmega328P__) || \
    defined(__AVR_ATmega328PB__) || \
    defined(__AVR_ATmega2560__)
ISR(TIMER0_OVF_vect)
#endif
{
    Time.irq();
}
