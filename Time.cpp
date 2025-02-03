#include "Time.h"

/*!
 * @brief  __Time__ constructor
 */
__Time__::__Time__(volatile uint8_t* tccrb, \
                   volatile uint8_t* timsk)
{
    this->tccrb = tccrb;
    this->timsk = timsk;
}

/*!
 * @brief  __Time__ destructor
 */
__Time__::~__Time__()
{
    this->tccrb = NULL;
    this->timsk = NULL;
}

/*!
 * @brief  Begins the time implementation
 * @return Returns 0 if time implementation already began, otherwise returns 1
 */
const uint8_t __Time__::begin(void)
{
    if (this->began)
        return (0);
    this->began = 1;
    
    sei();

    #if defined(__AVR_ATmega328__) || \
        defined(__AVR_ATmega328P__) || \
        defined(__AVR_ATmega328PB__) || \
        defined(__AVR_ATmega2560__)
    *this->tccrb = *this->tccrb & ~((1 << CS02) | (1 << CS00)); // Select clock prescaler to 8
    *this->tccrb = *this->tccrb | (1 << CS01);
    *this->timsk = *this->timsk | (1 << TOIE0);                 // Enable Timer Overflow Interrupt
    #else
    #error "Can not config Timer 0 in begin()"
    #endif
    return (1);
}

/*!
 * @brief  Resets the time counter
 */
void __Time__::reset(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        this->counter = 0;
}

/*!
 * @brief  Delays a certain amount of seconds
 * @param  s
 *         Amount of seconds to delay the <MCU>
 */
void __Time__::delay(const uint32_t s)
{
    const uint32_t timestamp = this->seconds();
    while ((this->seconds() - timestamp) < s);
}

/*!
 * @brief  Delays a certain amount of milliseconds
 * @param  ms
 *         Amount of milliseconds to delay the <MCU>
 */
void __Time__::delayMillis(const uint32_t ms)
{
    const uint32_t timestamp = this->milliseconds();
    while ((this->milliseconds() - timestamp) < ms);
}

/*!
 * @brief  Getting the amount of seconds elapsed since time implementation started
 * @return Returns the amount of seconds elapsed since time implementation started
 */
const uint32_t __Time__::seconds(void)
{
    return (this->milliseconds() / 1000);
}

/*!
 * @brief  Getting the amount of milliseconds elapsed since time implementation started
 * @return Returns the amount of milliseconds elapsed since time implementation started
 */
const uint32_t __Time__::milliseconds(void)
{
    return (this->microseconds() / 1000);
}

/*!
 * @brief  Getting the amount of microseconds elapsed since time implementation started
 * @return Returns the amount of microseconds elapsed since time implementation started
 */
const uint32_t __Time__::microseconds(void)
{
    uint32_t time;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        time = this->counter;
    return (time);
}
/*!
 * @brief  Ends the time implementation
 * @return Returns 0 if time implementation already ended, otherwise returns 1
 */
const uint8_t __Time__::end(void)
{
    if (!this->began)
        return (0);
    this->began = 0;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
    #if defined(__AVR_ATmega328__) || \
        defined(__AVR_ATmega328P__) || \
        defined(__AVR_ATmega328PB__) || \
        defined(__AVR_ATmega2560__)
    *this->tccrb = *this->tccrb & ~((1 << CS02) | (1 << CS01) | (1 << CS00)); // Clear clock prescaler to 0
    *this->timsk = *this->timsk & ~(1 << TOIE0);                              // Disable Timer Overflow Interrupt
    #else
    #error "Can not config Timer 0 in end()"
    #endif
    }
    this->reset();
	return (1);
}

/*!
 * @brief  The custom interrupt service routine implementation
 */
void __Time__::isr(void)
{
    this->counter = (this->counter + TIME_INCREMENT_VALUE);
}

#if defined(__AVR_ATmega328__) || \
    defined(__AVR_ATmega328P__) || \
    defined(__AVR_ATmega328PB__) || \
    defined(__AVR_ATmega2560__)
__Time__ Time = __Time__(&TCCR0B, &TIMSK0);
#else
#error "Can't create instance of Time"
#endif

#if defined(__AVR_ATmega328__) || \
    defined(__AVR_ATmega328P__) || \
    defined(__AVR_ATmega328PB__) || \
    defined(__AVR_ATmega2560__)
ISR(TIMER0_OVF_vect)
{
    Time.isr();
}
#else
#error "Can't create Timer 0  overflow interrupt routine"
#endif
