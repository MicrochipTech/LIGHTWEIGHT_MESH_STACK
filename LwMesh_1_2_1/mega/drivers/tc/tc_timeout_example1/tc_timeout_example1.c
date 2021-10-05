/**
 * \file
 *
 * \brief megaAVR Timer/Counter (TC) Timeout Driver Example 1
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

/**
 * \mainpage
 *
 * \section intro Introduction
 * This simple example shows how to use the \ref tc_timeout_group to toggle
 * two LEDs at different speeds.
 *
 * \section files Main files:
 *  - tc_timeout_example1.c: This example application
 *  - tc_timeout.c: TC Timeout Driver implementation
 *  - tc_timeout.h: TC Timeout Driver definitions
 *  - conf_tc_timeout.h: Configuration of the TC Timeout Driver
 *
 * \section driverinfo TC Timeout Driver
 * The driver can be found \ref tc_timeout_group "here".
 *
 * \section deviceinfo Device Info
 * All megaAVR devices with a Timer/Counter module can be used.
 *
 * \section setup Setup
 * The following connections are required on STK600 for this example:
 * - For LED output, connect PORTD pin header to LED pin header with a 10-pin
 *   cable.
 * - For 32KHz clock, connect a wire between 32KHz and TOSC2 on the AUX pin
 *   header.
 *
 * \section exampledescription Description of the example
 * The example will configure a tick speed of 1024 Hz and use two different
 * periodic timeouts to toggle two different LEDs. One will toggle at 1Hz and
 * the other at 1/3Hz.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.microchip.com/">Microchip</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include "compiler.h"
#include "tc_timeout.h"

// Use an enum to differentiate between the different timeouts
enum {
	EXAMPLE_TIMEOUT_SLOW,
	EXAMPLE_TIMEOUT_FAST,
};

#define LED0_MASK (1 << 0)
#define LED1_MASK (1 << 1)

/**
 * \brief Toggle LED
 *
 * Newer megaAVR devices can toggle a I/O pin by writing to PIN register, while on
 * older ones this needs to be done by a read-modify-write operation. This function
 * abstract these differences.
 *
 * \param mask Mask of port LEDs to toggle
 */
static inline void toggle_led(uint8_t mask)
{
#if defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) \
		|| defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
	/*
	 * For older megaAVR devices read-modify-write PORT register.
	 * This isn't safe for interrupts.
	 */
	PORTD ^= mask;
#else
	// Use PIN register to toggle on newer megaAVR devices
	PIND = mask;
#endif
}

int main(void)
{
	// Set LED0 and LED1 connected to port D to outputs
	DDRD |= LED0_MASK | LED1_MASK;

	// Initialize the timeout internals before using it
	tc_timeout_init();

	// Enable global interrupts
	sei();

	/*
	 * Start the two timeouts. One at 1 second interval and the other at
	 * 1/3 second interval.
	 */
	tc_timeout_start_periodic(EXAMPLE_TIMEOUT_SLOW, TC_TIMEOUT_TICK_HZ);
	tc_timeout_start_periodic(EXAMPLE_TIMEOUT_FAST, TC_TIMEOUT_TICK_HZ / 3);

	while (1) {
		// Loop and test for each expire and toggle LED
		if (tc_timeout_test_and_clear_expired(EXAMPLE_TIMEOUT_SLOW)) {
			toggle_led(LED0_MASK);
		}
		if (tc_timeout_test_and_clear_expired(EXAMPLE_TIMEOUT_FAST)) {
			toggle_led(LED1_MASK);
		}
	}
}

