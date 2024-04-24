/*
 * Time - Demo.cpp
 *
 * Created: 4/4/2024 11:48:25 AM
 * Author : rotes
 */ 

/* Dependencies */
#include "Time/Time.h"
#include "GPIO/GPIO.h"

void configureClock(void);

int main(void)
{
    configureClock();
    Time.begin();
    PORTC.DIR = PORTC.DIR | PIN0_bm;
    while (1) 
    {
        const uint32_t currentTime = Time.milliseconds();
        static uint32_t previousTime = 0;

        if ((currentTime - previousTime) >= 100)
        {
            PORTC.OUT = PORTC.OUT ^ PIN0_bm;
            previousTime = currentTime;
        }
    }
}

void configureClock(void)
{
	// Enable the 32 MHz internal oscillator
	OSC.CTRL |= OSC_RC32MEN_bm;
	while (!(OSC.STATUS & OSC_RC32MRDY_bm)); // Wait for the oscillator to stabilize

	// Enable the PLL
	OSC.PLLCTRL = OSC_PLLSRC_RC32M_gc | OSC_PLLFAC3_bm;
	OSC.CTRL |= OSC_PLLEN_bm;

	// Wait for PLL to stabilize
	while (!(OSC.STATUS & OSC_PLLRDY_bm));

	// Set the system clock to use the PLL
	CCP = CCP_IOREG_gc; // Enable CCP protection
	CLK.CTRL = CLK_SCLKSEL_PLL_gc; // Select the PLL as the system clock
}

