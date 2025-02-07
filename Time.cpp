#include "Time.h"

/**
 * @brief  Constructor for the __Time__ class.
 * @param  tccrb  Pointer to the TCCRB register for configuring the timer.
 * @param  timsk  Pointer to the TIMSK register for managing interrupt flags.
 * @note   This constructor initializes the class with references to timer control and interrupt registers.
 */
__Time__::__Time__(volatile uint8_t* tccrb, \
                   volatile uint8_t* timsk)
{
    this->tccrb = tccrb;
    this->timsk = timsk;
}


/**
 * @brief  Destructor for the __Time__ class.
 * @details This destructor clears the references to the TCCRB and TIMSK registers.
 *          It is used to clean up the class instance when it is no longer needed.
 */
__Time__::~__Time__()
{
    this->tccrb = NULL;
    this->timsk = NULL;
}


/**
 * @brief  Initializes the timer and enables the overflow interrupt.
 * @details This function configures the timer with a prescaler of 8 and enables the
 *          Timer Overflow Interrupt for the given MCU. It also ensures that global
 *          interrupts are enabled by calling sei().
 *          It should be called once to initialize the timer before using it.
 * 
 * @return 1 if the initialization was successful, 0 if the timer was already initialized.
 */
const uint8_t __Time__::begin(void)
{
    if (this->began)
        return (0);  /**< If the timer has already been initialized, exit the function */

    this->began = 1;  /**< Set the 'began' flag to indicate that initialization has occurred */
    
    sei();  /**< Enable global interrupts by calling the sei() function */

    #if defined(__AVR_ATmega328__) || \
        defined(__AVR_ATmega328P__) || \
        defined(__AVR_ATmega328PB__) || \
        defined(__AVR_ATmega2560__)
    
    *this->tccrb = *this->tccrb & ~((1 << CS02) | (1 << CS00));  /**< Clear the CS02 and CS00 bits to set the prescaler to 8 */
    *this->tccrb = *this->tccrb | (1 << CS01);                    /**< Set the CS01 bit to complete the configuration of the prescaler to 8 */
    
    *this->timsk = *this->timsk | (1 << TOIE0);                    /**< Enable the Timer Overflow Interrupt by setting the TOIE0 bit in the TIMSK register */
    
    #else
    #error "Can not config Timer 0 in begin()"  /**< Display an error if the MCU is not supported */
    #endif

    return (1);  /**< Return 1 to indicate that the initialization was successful */
}


/**
 * @brief  Resets the timer counter to zero.
 * @details This function atomically resets the timer counter to 0. The use of 
 *          ATOMIC_BLOCK ensures that the operation is performed without 
 *          interruption, preserving the integrity of the counter value during 
 *          execution.
 */
void __Time__::reset(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)  /**< Enter atomic block to ensure no interrupt occurs during this operation */
        this->counter = 0;  /**< Reset the timer counter to 0 */
}


/**
 * @brief  Delays execution for a specified number of seconds.
 * @param  s The number of seconds to delay.
 * @details This function stores the current time in seconds, then enters a 
 *          loop, repeatedly checking the current time until the specified 
 *          number of seconds have passed. It uses the `seconds()` method to 
 *          get the current time and compares it to the stored timestamp.
 */
void __Time__::delay(const uint32_t s)
{
    const uint32_t timestamp = this->seconds();  /**< Store the current time in seconds */
    while ((this->seconds() - timestamp) < s);  /**< Wait until the elapsed time is greater than or equal to the specified delay time */
}


/**
 * @brief  Delays execution for a specified number of milliseconds.
 * @param  ms The number of milliseconds to delay.
 * @details This function stores the current time in milliseconds, then enters 
 *          a loop, repeatedly checking the current time until the specified 
 *          number of milliseconds have passed. It uses the `milliseconds()` 
 *          method to get the current time and compares it to the stored timestamp.
 */
void __Time__::delayMillis(const uint32_t ms)
{
    const uint32_t timestamp = this->milliseconds();  /**< Store the current time in milliseconds */
    while ((this->milliseconds() - timestamp) < ms);  /**< Wait until the elapsed time is greater than or equal to the specified delay time */
}


/**
 * @brief  Returns the current time in seconds.
 * @details This function divides the current time in milliseconds (retrieved 
 *          via the `milliseconds()` function) by 1000 to convert the time 
 *          into seconds.
 * @return The current time in seconds.
 */
const uint32_t __Time__::seconds(void)
{
    return (this->milliseconds() / 1000);  /**< Convert milliseconds to seconds */
}


/**
 * @brief  Returns the current time in milliseconds.
 * @details This function divides the current time in microseconds (retrieved 
 *          via the `microseconds()` function) by 1000 to convert the time 
 *          into milliseconds.
 * @return The current time in milliseconds.
 */
const uint32_t __Time__::milliseconds(void)
{
    return (this->microseconds() / 1000);  /**< Convert microseconds to milliseconds */
}


/**
 * @brief  Returns the current time in microseconds.
 * @details This function retrieves the current time from the `counter` variable.
 *          The value of `counter` is read atomically to prevent interruptions 
 *          during the process, ensuring the value is accurate.
 * @return The current time in microseconds.
 */
const uint32_t __Time__::microseconds(void)
{
    uint32_t time;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)  /**< Ensure atomic read of the counter value */
        time = this->counter;
    return (time);  /**< Return the time in microseconds */
}


/**
 * @brief  Disables the timekeeping and resets relevant registers.
 * @details This function stops the timekeeping by clearing the clock prescaler, 
 *          disabling the timer overflow interrupt, and resetting the `counter`. 
 *          It ensures the timekeeping functionality is stopped safely by using 
 *          atomic operations to prevent interruptions during the configuration 
 *          of the timer registers.
 * @return 1 if successful, 0 if the timekeeping was already disabled.
 */
const uint8_t __Time__::end(void)
{
    if (*this->began)  /**< Ensure timekeeping was previously started */
        return (0);     /**< Return 0 if timekeeping is already ended */
    this->began = 0;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)  /**< Ensure atomic changes to timer configuration */
    {
    #if defined(__AVR_ATmega328__) || \
        defined(__AVR_ATmega328P__) || \
        defined(__AVR_ATmega328PB__) || \
        defined(__AVR_ATmega2560__)
    *this->tccrb = *this->tccrb & ~((1 << CS02) | (1 << CS01) | (1 << CS00));  /**< Clear clock prescaler to 0 */
    *this->timsk = *this->timsk & ~(1 << TOIE0);  /**< Disable Timer Overflow Interrupt */
    #else
    #error "Can not config Timer 0 in end()"
    #endif
    }
    this->reset();  /**< Reset the counter value */
    return (1);  /**< Return 1 to indicate success */
}


/**
 * @brief  Interrupt Service Routine (ISR) for timer overflow.
 * @details This function is triggered when the timer overflows. It increments the 
 *          `counter` by a predefined time increment value (`TIME_INCREMENT_VALUE`), 
 *          which is typically used to track time in microseconds or milliseconds. 
 *          This ISR is called automatically during timer overflow events and 
 *          ensures that the timekeeping is updated consistently.
 */
void __Time__::isr(void)
{
    this->counter = (this->counter + TIME_INCREMENT_VALUE);  /**< Increment the counter by the time increment value */
}


/**
 * @brief  Conditional initialization of the Time object based on the target MCU.
 * @details This code checks if the target microcontroller is one of the supported 
 *          AVR models (ATmega328, ATmega328P, ATmega328PB, or ATmega2560). If it is, 
 *          an instance of the `__Time__` class, named `Time`, is created and 
 *          initialized with pointers to the timer control and interrupt registers 
 *          (`TCCR0B` and `TIMSK0`).
 *          If the target MCU is not one of the listed models, a preprocessor error 
 *          is triggered, indicating that the `Time` instance cannot be created.
 */
#if defined(__AVR_ATmega328__) || \
    defined(__AVR_ATmega328P__) || \
    defined(__AVR_ATmega328PB__) || \
    defined(__AVR_ATmega2560__)
__Time__ Time = __Time__(&TCCR0B, &TIMSK0);  /**< Create and initialize the Time object for the supported MCUs */
#else
#error "Can't create instance of Time"  /**< Error: Target MCU is not supported for Time object creation */
#endif


/**
 * @brief  Timer 0 Overflow Interrupt Service Routine (ISR).
 * @details This ISR is executed when Timer 0 overflows, and it calls the `isr()` 
 *          method of the `Time` object to update the time counter. 
 *          The ISR is conditionally compiled based on whether the target MCU 
 *          is one of the supported AVR models (ATmega328, ATmega328P, ATmega328PB, 
 *          or ATmega2560). If the target MCU is not one of these, an error is 
 *          triggered indicating that the Timer 0 overflow ISR cannot be created.
 */
#if defined(__AVR_ATmega328__) || \
    defined(__AVR_ATmega328P__) || \
    defined(__AVR_ATmega328PB__) || \
    defined(__AVR_ATmega2560__)
ISR(TIMER0_OVF_vect)  /**< Timer 0 Overflow Interrupt Service Routine */
{
    Time.isr();  /**< Call the isr() method of the Time object to update the time counter */
}
#else
#error "Can't create Timer 0 overflow interrupt routine"  /**< Error: Target MCU does not support Timer 0 overflow ISR */
#endif

